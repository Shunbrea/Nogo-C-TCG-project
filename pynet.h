#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <tuple>
//#include <board.h>

class PyNNet {

public:    
    PyObject *pName, *pModule, *pCls, *pFunc;
    PyObject *pArgs, *pValue, *pClsIns, *pResult;


    // constructor
    PyNNet(){        
        Py_Initialize();
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append(\".\")");
        pName = PyUnicode_DecodeFSDefault("nnet");
        cerr << "PyNNet is init" << endl;
        pModule = PyImport_Import(pName);
        if (pModule == NULL) { cerr << "pModule is NULL" << endl;}
        pCls = PyObject_GetAttrString(pModule, "pynet");
        if (pCls == NULL) { cerr << "pCls is NULL" << endl;}
        //pArgs = PyTuple_New(0);
        pClsIns = PyObject_CallObject(pCls, nullptr);
        if (pClsIns == NULL) { cerr << "pClsIns is NULL" << endl;}
        pFunc = PyObject_GetAttrString(pClsIns, "predict");
        if (pFunc == NULL) { cerr << "pFunc is NULL" << endl;}

        //Py_DECREF(pArgs);
        Py_DECREF(pName);
        Py_DECREF(pModule);
    };
    
    tuple<std::array, float> predict(board board) {
        pArgs = PyTuple_New(81);
        for (int i = 0; i < 81; i++){
            pValue = PyLong_FromSize_t(board(i));
            PyTuple_SetItem(pArgs, i, pValue);
        }
        pResult = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);

        PyArrayObject pProbs = PyTuple_GetItem(pResult, 0);
        pValue = PyTuple_GetItem(pResult, 1);

        int row = pProbs->dimensions[0];
        std::array<float, row> prob;
        for (size_t i = 1; i < row; i++){
            prob[i] = pProbs->data + i * pProbs->stride[0];
            cout << prob[i] << endl;
        }
        Py_DECREF(pProbs);

        float value = PyFloat_AsDouble(pValue);
        Py_DECREF(pValue);

        return prob, value;
    };
};

