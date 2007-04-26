#include "numeric/nlpmodel.hxx"
#include "myoptimizer.hxx"
#include "myfunctor.hxx"

#include <vector>

using namespace scsolver::numeric;
using namespace scsolver::numeric::nlp;
using std::vector;

int main (int argc, char *argv[])
{
    // Instantiate model and optimizer classes.
    Model* model         = new Model();
    BaseAlgorithm* opt   = new MyOptimizer();
    BaseFuncObj* functor = new MyFunctor();

    // The model has two variables.
    model->pushVar(0.0);
    model->pushVar(0.0);
    model->setFuncObject(functor);
    model->setVarBound(0, BOUND_LOWER,  1.0);
    model->setVarBound(0, BOUND_UPPER, 23.5);
    model->setVarBound(1, BOUND_LOWER,  0.0);
    model->setGoal(GOAL_MINIMIZE);
    model->print();

    // The optimization class receives the model, then optimizes it.  In 
    // production code, the solve() call needs to be surrounded with a 
    // try block because when the model is infeasible or something bad 
    // happens, the optimizer class throws an exception.  So, we need to 
    // catch it.
    opt->setModel(model);
    opt->solve();

    // Once the optimization is complete, get the solution.
    const vector<double>& sol = opt->getSolution();

    printf("solution is: \n");
    size_t n = sol.size();
    for (size_t i = 0; i < n; ++i)
        printf("  x%d = %.2f\n", i+1, sol[i]);

    // Destroy class instances.
    delete opt;
    delete functor;
    delete model;
}
