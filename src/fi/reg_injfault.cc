#include "fi/faultq.hh"
#include "fi/reg_injfault.hh"
#include "fi/fi_system.hh"


#include "sim/full_system.hh"

#include "arch/utility.hh"
using namespace std;


RegisterInjectedFault::RegisterInjectedFault(ifstream &os)
	: CPUInjectedFault(os)
{
	setFaultType(InjectedFault::RegisterInjectedFault);
	std::string v;
	int k;
	os>>v;
	setRegType(v);
	os>>k;
	setRegister(k);
	fi_system->mainInjectedFaultQueue.insert(this);
}

RegisterInjectedFault::~RegisterInjectedFault()
{
}


const char *
RegisterInjectedFault::description() const
{
    return "RegisterInjectedFault";
}



void RegisterInjectedFault::dump() const
{
  if (DTRACE(FaultInjection)) {
    std::cout << "===RegisterInjectedFault::dump()===\n";
    CPUInjectedFault::dump();
    std::cout << "\tregister: " << getRegister() << "\n";
    std::cout << "\tregType: " << getRegType() << "\n";
    std::cout << "~==RegisterInjectedFault::dump()===\n";
  }
}




int
RegisterInjectedFault::process()
{
  DPRINTF(FaultInjection, "===RegisterInjectedFault::process() ID: %d ===\n", getFaultID());
  dump();
  switch (getRegType())
    {
    case(RegisterInjectedFault::IntegerRegisterFault):
      {
	TheISA::IntReg regval = getCPU()->getContext(getTContext())->readIntReg(getRegister());
	TheISA::IntReg mask = manifest(regval, getValue(), getValueType());
	getCPU()->getContext(getTContext())->setIntReg(getRegister(), mask);
// 	fi_system->add_altered_int_reg(getRegister());
	break;
      }
    case(RegisterInjectedFault::FloatRegisterFault):
      {
	TheISA::FloatReg regval = getCPU()->getContext(getTContext())->readFloatReg(getRegister());
	TheISA::FloatReg mask = manifest(regval, getValue(), getValueType());
	getCPU()->getContext(getTContext())->setFloatReg(getRegister(), mask);
// 	fi_system->add_altered_float_reg(getRegister());
	break;
      }
    case(RegisterInjectedFault::MiscRegisterFault):
      {
	TheISA::MiscReg regval = getCPU()->getContext(getTContext())->readMiscReg(getRegister());
	TheISA::MiscReg mask = manifest(regval, getValue(), getValueType());
	getCPU()->getContext(getTContext())->setMiscReg(getRegister(), mask);
// 	fi_system->add_altered_misc_reg(getRegister());
	break;
      }
    default:
      {
	std::cout << "RegisterInjectedFault::process() -- Default case getRegType() Error\n";
	assert(0);
	break;
      }
    }
  

  check4reschedule();

  DPRINTF(FaultInjection, "~==RegisterInjectedFault::process() ID: %d ===\n", getFaultID());

  return 0;
}

