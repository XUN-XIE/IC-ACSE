#include "CCircuit.h"
#include <iostream>
#include <cmath>
#include <algorithm> 

using namespace std;

void CCircuit::Setup_Units(int num)
{
	if (unit != nullptr)
		delete[] unit;

	num_units = num;
	unit = new CUnit[num];
}

void CCircuit::Clear_Units()
{
	for (int i = 0; i < num_units; i++)
		unit[i].clear();

	Conc.clear();
	Tails.clear();
}

void CCircuit::Clear_Marks()
{
	for (int i = 0; i < num_units; i++)
		unit[i].mark = false;
}

void CCircuit::copy_clear_feeds()
{
	for (int i = 0; i < num_units; i++)
	{
		unit[i].Feed_Old = unit[i].Feed;
		unit[i].Feed.clear();
	}

	Conc_Old = Conc;
	Conc.clear();
	Tails_Old = Tails;
	Tails.clear();
}

void CCircuit::Setup_From_Vector(int *vector)
{
	Clear_Units();

	feed_num = vector[0];

	for (int i = 0; i < num_units; i++)
	{
		unit[i].conc_num = vector[i * 2 + 1];
		unit[i].tails_num = vector[i * 2 + 2];

		if (unit[i].conc_num == num_units)
			unit[i].conc_num = -1;
		if (unit[i].conc_num == num_units+1)
			unit[i].conc_num = -2;

		if (unit[i].tails_num == num_units)
			unit[i].tails_num = -1;
		if (unit[i].tails_num == num_units + 1)
			unit[i].tails_num = -2;
	}
}

void CCircuit::mark_forward(int cell)
{
	unit[cell].mark = true;
	if (unit[cell].conc_num >= 0 && !unit[unit[cell].conc_num].mark)
		mark_forward(unit[cell].conc_num);

	if (unit[cell].tails_num >= 0 && !unit[unit[cell].tails_num].mark)
		mark_forward(unit[cell].tails_num);
}

void CCircuit::set_initial_forward(int cell)
{
	unit[cell].mark = true;
	unit[cell].Calculate_Unit();
	if (unit[cell].conc_num >= 0 && !unit[unit[cell].conc_num].mark)
	{
		unit[unit[cell].conc_num].Feed = unit[cell].Conc;
		set_initial_forward(unit[cell].conc_num);
	}

	if (unit[cell].tails_num >= 0 && !unit[unit[cell].tails_num].mark)
	{
		unit[unit[cell].tails_num].Feed = unit[cell].Tails;
		set_initial_forward(unit[cell].tails_num);
	}
}

void CCircuit::set_initial_feed()
{
	for (int i = 0; i < num_units; i++)
	{
		unit[i].Feed = Feed;
		unit[i].Calculate_Unit();
	}

}

bool CCircuit::see_exit(int cell, int exit_num)
{
	bool test1, test2;
	unit[cell].mark = true;

	if (unit[cell].conc_num >= 0 && !unit[unit[cell].conc_num].mark)
		test1 = see_exit(unit[cell].conc_num, exit_num);
	else if (unit[cell].conc_num < 0)
		test1 = (unit[cell].conc_num == exit_num);
	else
		test1 = false;

	if (unit[cell].tails_num >= 0 && !unit[unit[cell].tails_num].mark)
		test2 = see_exit(unit[cell].tails_num, exit_num);
	else if (unit[cell].tails_num < 0)
		test2 = (unit[cell].tails_num == exit_num);
	else
		test2 = false;

	return test1 || test2;
}

bool CCircuit::Check_Valid()
{
	//Check for self-recycle
	for (int i = 0; i < num_units; i++)
		if (unit[i].conc_num == i || unit[i].tails_num == i)
			return false;

	//Check that all cells can see the feed
	Clear_Marks();
	mark_forward(feed_num);

	for (int i = 0; i < num_units; i++)
		if (!unit[i].mark)
			return false;

	//check that all cells can see an outlet
	for (int i = 0; i < num_units; i++)
	{
		Clear_Marks();
		if (!see_exit(i, -1))
			return false;
		Clear_Marks();
		if (!see_exit(i, -2))
			return false;
	}

	return true;
}

double CCircuit::Run_Simulation(double tot_rel, int max_it, bool &diverged)
{
	double max_rel_change;

	diverged = false;

	Clear_Units();

	//Give All streams an initial guess
	unit[feed_num].Feed = Feed;
	set_initial_forward(feed_num);

	int cnt=0;

	do
	{
		for (int i = 0; i < num_units; i++)
			unit[i].Calculate_Unit();

		copy_clear_feeds();

		unit[feed_num].Feed = Feed;

		for (int i = 0; i < num_units; i++)
		{
			int cell_forward;
			if ((cell_forward = unit[i].conc_num) >= 0)
				unit[cell_forward].Feed = unit[cell_forward].Feed + unit[i].Conc;
			else if (cell_forward == -1)
				Conc = Conc + unit[i].Conc;
			else
				Tails = Tails + unit[i].Conc;

			if ((cell_forward = unit[i].tails_num) >= 0)
				unit[cell_forward].Feed = unit[cell_forward].Feed + unit[i].Tails;
			else if (cell_forward == -1)
				Conc = Conc + unit[i].Tails;
			else
				Tails = Tails + unit[i].Tails;
		}

		max_rel_change = 0.0;

		for (int i = 0; i < num_units; i++)
			for (int j = 0; j < CStream::num_components; j++)
			{
				max_rel_change = max(fabs((unit[i].Feed[j] - unit[i].Feed_Old[j]) / unit[i].Feed_Old[j]), max_rel_change);

				if (unit[i].Feed[j] > Feed[j] * 1000)			//Divergence test
					cnt = max_it;
			}

		for (int j = 0; j < CStream::num_components; j++)
		{
			max_rel_change = max(fabs((Conc[j] - Conc_Old[j]) / Conc_Old[j]), max_rel_change);
			max_rel_change = max(fabs((Tails[j] - Tails_Old[j]) / Tails_Old[j]), max_rel_change);
		}

		cnt++;
	} while (max_rel_change > tot_rel && cnt<max_it);

	double performance=0.0;

	if (cnt >= max_it)
	{
		diverged = true;
		//worst possible performance
		for (int j = 0; j < CStream::num_components; j++)
			if (conc_value[j] < 0.0)
				performance += Feed[j] * conc_value[j];
	}
	else
	{
		for (int j = 0; j < CStream::num_components; j++)
		{
			performance += Conc[j] * conc_value[j];
		}
	}
	return performance;
}


//Not that the circuit parameters are currently hard coded
void CCircuit::Set_Cicuit_Parameters()
{
	Feed[0] = 10.0;
	Feed[1] = 100.0;

	conc_value[0] = 100.0;
	conc_value[1] = -500.0;

	CUnit::Setup();
	CUnit::proportion_to_conc[0] = 0.2;
	CUnit::proportion_to_conc[1] = 0.05;
}
