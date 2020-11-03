#include "CUnit.h"

int CStream::num_components = 2;
double *CUnit::proportion_to_conc = nullptr;

CStream CStream::operator+(const CStream &a)
{
	CStream temp;
	for (int i = 0; i < CStream::num_components; i++)
		temp.M_component[i] = a.M_component[i] + M_component[i];

	return temp;
}

void CUnit::clear()
{
	Feed.clear();
	Feed_Old.clear();
	Tails.clear();
	Conc.clear();
	mark = false;
}


void CUnit::Calculate_Unit()
{
	for (int i = 0; i < CStream::num_components; i++)
	{
		Conc.M_component[i] = Feed.M_component[i] * proportion_to_conc[i];
		Tails.M_component[i] = Feed.M_component[i] * (1.0 - proportion_to_conc[i]);
	}
}