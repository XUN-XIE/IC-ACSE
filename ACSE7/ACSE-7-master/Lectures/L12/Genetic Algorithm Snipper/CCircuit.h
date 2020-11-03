#pragma once
#include "CUnit.h"

class CCircuit
{
public:
	int num_units;

	int feed_num;

	CUnit *unit;
	CStream Feed, Conc, Tails;
	CStream Conc_Old, Tails_Old;

	double *conc_value;

	CCircuit()
	{
		unit = nullptr;
		conc_value = new double[CStream::num_components];
	}

	~CCircuit()
	{
		delete[] unit;
		delete[] conc_value;
	}

	void Setup_Units(int num);
	void Clear_Units();
	void Clear_Marks();
	void Set_Cicuit_Parameters();
	bool Check_Valid();
	double Run_Simulation(double tot_rel, int max_it, bool &diverged);
	void Setup_From_Vector(int *vector);

private:
	void mark_forward(int cell);
	void set_initial_forward(int cell);
	void set_initial_feed();
	bool see_exit(int cell, int exit_num);
	void copy_clear_feeds();
};