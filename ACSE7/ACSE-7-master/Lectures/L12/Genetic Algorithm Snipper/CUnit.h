#pragma once

class CStream
{
public:
	static int num_components;

	double *M_component;

	CStream()
	{
		M_component = new double[num_components];
		for (int i = 0; i < num_components; i++)
			M_component[i] = 0.0;
	}

	CStream(CStream &a)
	{
		M_component = new double[num_components];
		for (int i = 0; i < num_components; i++)
			M_component[i] = a.M_component[i];
	}

	~CStream()
	{
		delete[] M_component;
	}

	void clear(void)
	{
		for (int i = 0; i < num_components; i++)
			M_component[i] = 0.0;
	}

	CStream operator+(const CStream &a);
	CStream& operator=(const CStream &a)
	{
		if (this != &a)
		{
			for (int i = 0; i < num_components; i++)
				M_component[i] = a.M_component[i];
		}
		return *this;
	}

	double& operator[](int i)
	{
		return M_component[i];
	}
};


class CUnit
{
public:
	CStream Feed, Conc, Tails;
	CStream Feed_Old;
	int conc_num, tails_num;
	bool mark;

	static double *proportion_to_conc;

	static void Setup()
	{
		proportion_to_conc = new double[CStream::num_components];
	}

	static void Close_All()
	{
		delete[] proportion_to_conc;
	}

	void Calculate_Unit();
	void clear(void);
};
