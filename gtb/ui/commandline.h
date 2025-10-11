
#ifndef GTB_COMMANDLINE_H
#define GTB_COMMANDLINE_H

#include <vector>
#include <gtb/common.hpp>

GTB_BEGIN_NAMESPACE

class CommandLine {

public:
	CommandLine(int argc, char **argv, char *usage) : _argc(argc), _argv(argv), _usage(usage) { }

	void AddVariable(char *name, bool *var, char *help);
	void AddVariable(char *name, int *var, char *help);
	void AddVariable(char *name, float *var, char *help);
	void AddVariable(char *name, double *var, char *help);
	void AddVariable(char *name, char **var, char *help);

    typedef void (*PFUN)();
    typedef int (*PFUN2)(int argc, char** argv);
	void AddFunction(char *name, PFUN,  char *help);
	void AddFunction(char *name, PFUN2, char *help);

    void Print();
	void Parse(int start);

private:
	int _argc;
	char **_argv;
	char *_usage;


	enum OptionType {
		CharVariable,
		BoolVariable,
		IntVariable,
		FloatVariable,
		DoubleVariable,
		Function,
		FunctionNoArgs
	};


	struct Option {
		char *name;
		OptionType type;
		PFUN  pfun;
		PFUN2 pfun2;
		void *varp;
		char *help;
	};

	std::vector<Option> _options;

};

#define CL_ADD_VAR(cl,name,help) cl.AddVariable(#name, &name, help)
#define CL_ADD_FUN(cl,name,help) cl.AddFunction(#name, do_##name, help)

GTB_END_NAMESPACE

#endif
