#include "cSquareMatrix.hh"


cSquareMatrix::cSquareMatrix(const int& size)
	:_size(size), _data(_size* _size, 0) {}

cSquareMatrix::cSquareMatrix(const cSquareMatrix& m)
	: _size(m._size), _data(m._data) {}

cSquareMatrix& 
cSquareMatrix::operator=(const cSquareMatrix& m)
{
	_size = m._size;
	_data = m._data;
	return *this;
}

double& 
cSquareMatrix::operator()(const int& i, const int& j) 
{
    return _data[i*_size + j];
}

double 
cSquareMatrix::operator()(const int& i, const int& j) const 
{
    return _data[i*_size + j];
}

int 
cSquareMatrix::GetSize() const 
{
    return _size;
}

void 
cSquareMatrix::ShowMatrix() const
{//Display contents of a square matrix
	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _size; j++)
		{
			cout << operator()(i, j) << "   ";
		}
		cout << endl;
	}
}

cSquareMatrix& 
cSquareMatrix::operator*=(const cSquareMatrix& m)
{//Matrix multiplication
	cSquareMatrix res(*this);
	double s;
	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _size; j++)
		{
			s = 0;
			for (int k = 0; k < _size; k++)
			{
				s += res(i, k) * m(k, j);
			}
			operator()(i, j) = s;
		}
	}
	return *this;
}

unique_ptr<cSquareMatrix>
cSquareMatrix::operator*(const cSquareMatrix& m) const
{
	unique_ptr<cSquareMatrix> res(new cSquareMatrix(*this));
	*res *= m;
	return res;
}

unique_ptr<vector<double>> 
cSquareMatrix::operator*(const vector<double>& v) const
{//Product of matrix and vector
	unique_ptr<vector<double>> res(new vector<double>(_size, 0));
	double s;
	for (int i = 0; i < _size; i++)
	{
		s = 0;
		for (int k = 0; k < _size; k++)
		{
			s += operator()(i, k) * v[k];
		}
		(*res)[i] = s;
	}
	return res;
}

unique_ptr<cSquareMatrix>
cSquareMatrix::Transpose() const
{//Matrix transposition
	unique_ptr<cSquareMatrix> res(new cSquareMatrix(_size));
	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _size; j++)
		{
			(*res)(i, j) = operator()(j, i);
		}
	}
	return res;
}

unique_ptr<cSquareMatrix>
cSquareMatrix::Cholesky() const
{//Cholesky decomposition
	unique_ptr<cSquareMatrix> L(new cSquareMatrix(_size));
	double s;
	for (int i = 0; i < _size; i++)
	{
		for (int j = i; j < _size; j++)
		{
			s = 0;
			for (int k = 0; k < i; k++)
			{
				s += (*L)(i, k) * (*L)(j, k);
			}
			if (i == j) (*L)(i, j) = sqrt(operator()(i, j) - s);
			else (*L)(j, i) = (operator()(i, j) - s) / (*L)(i, i);
		}
	}
	return L;
}

void 
ShowVector(const vector<double>& v) 
{//Display contents of a column vector
    for (int i = 0; i < v.size(); i++) {
        cout << v[i] << endl;
    }
}

unique_ptr<vector<double>> 
operator+(const vector<double>& a, const vector<double>& b)
{//Sum of two vectors
	int n = a.size();
	unique_ptr<vector<double>> res(new vector<double>(a));
	for (int i = 0; i < n; i++)
		(*res)[i] += b[i];
	return res;
}

unique_ptr<vector<double>>
operator-(const vector<double>& a, const vector<double>& b)
{//Subtraction between two vectors
	int n = a.size();
	unique_ptr<vector<double>> res(new vector<double>(a));
	for (int i = 0; i < n; i++)
		(*res)[i] -= b[i];
	return res;
}

unique_ptr<vector<double>> 
operator*(double d, const vector<double>& v)
{//Product between a scalar and a vector
	int n = v.size();
	unique_ptr<vector<double>> res(new vector<double>(v));
	for (int i = 0; i < n; i++)
		(*res)[i] *= d;
	return res;
}