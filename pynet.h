#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <tuple>
#include <array>
//#include <board.h>

class PyNNet {

public:    
    PyObject *pName, *pModule, *pCls, *pFunc;
    PyObject *pArgs, *pClsIns, *pValue, *pResult;


    // constructor
    PyNNet(){        
        Py_Initialize();
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append(\".\")");
        pName = PyUnicode_DecodeFSDefault("nnet");
        cerr << "PyNNet is init" << endl;
        pModule = PyImport_Import(pName);
        if (pModule == NULL) { cerr << "pModule is NULL" << endl;}
        pCls = PyObject_GetAttrString(pModule, "pymcts");
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
    
    size_t GetAction(board board, board::piece_type who) {
        pArgs = PyTuple_New(81);
        int k = 0;
        for (int i = 0; i < 9; i++){
            for (int j = 0; j < 9; j++){
                if (who == board::black){
                    pValue = PyLong_FromSize_t(board[j][i]);
                } else {
                    if (board[j][i] == board::black){
                        pValue = PyLong_FromSize_t(board::white);
                    } else if (board[j][i] == board::white){
                        pValue = PyLong_FromSize_t(board::black);
                    } else {
                        pValue = PyLong_FromSize_t(board[j][i]);
                    }
                }
                PyTuple_SetItem(pArgs, k, pValue);
                k++;
            }
        }
        Py_DECREF(pValue);

        pResult = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);

        size_t value = PyFloat_AsDouble(pResult);
        Py_DECREF(pResult);

        // cout << "value = " << value << endl;
        size_t y = value / board::size_y;
        // cout << "y = " << y << endl;
        size_t x = value % board::size_x;
        // cout << "x = " << x << endl;
        value = x * board::size_x + y;
        // cout << "new value = " << value << endl;
        return value;
    };
};

