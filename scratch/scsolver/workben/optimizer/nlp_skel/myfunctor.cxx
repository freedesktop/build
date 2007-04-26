
#include "myfunctor.hxx"

using std::string;
using std::vector;

namespace scsolver {

namespace numeric { 

namespace nlp {

MyFunctor::MyFunctor() :
    mVars(2)
{
}

MyFunctor::~MyFunctor() throw()
{
}

double MyFunctor::eval()
{
    double x1 = mVars[0], x2 = mVars[1];
    return x1*x1 + x2*x1 + 10.0;
}

string MyFunctor::getFuncString() const
{
    string foo = "f(x1, x2) = x1*x1 + x2*x1 + 10";
    return foo;
}

const vector<double>& MyFunctor::getVars() const
{
    return mVars;
}

void MyFunctor::setVar(size_t index, double var)
{
    if (index >= 2)
        return;
    mVars.at(index) = var;
}

void MyFunctor::setVars(const vector<double>& vars)
{
    vector<double> tmp(vars);
    mVars.swap(tmp);
}

} // namespace nlp 
} // namespace numeric
} // namespace scsolver
