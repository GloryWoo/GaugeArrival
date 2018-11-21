/*******************************************************************************
 *  Project       GaugeArrival
 *  (c) copyright 2013
 *  Company       HARMAN International
 *                All rights reserved
 * Secrecy Level  STRICTLY CONFIDENTIAL
 ******************************************************************************/

#include <signal.h>
#include "GaugeArrival.hpp"
#include "TraceMacros.hpp"



static Poco::Event gExitService;
static bool terminated = false;

GaugeArrival * pGaugeArrive;

void sigHandler(int sig)
{
   CODE_TRACE_ENTER_METHOD();

   CODE_TRACE_DBG_MSG(("Receive system signal %d", sig));

   switch (sig)
   {
      case SIGINT:
      case SIGTERM:
      {
         if (!terminated)
         {
            CODE_TRACE_DBG_MSG(("Send quit signal to main loop."));
            //g_main_loop_quit(mainLoop);
         }

         terminated = true;
         gExitService.set();
         sleep(1);

         break;
      }

      default:
         break;
   }

   CODE_TRACE_EXIT_METHOD();
}

int main(int argc, char *argv[]) {
	CODE_TRACE_ENTER_METHOD();

	signal(SIGTERM, sigHandler);
	signal(SIGINT, sigHandler);
	pGaugeArrive = GaugeArrival::GetInstance();
	//pGaugeArrive->connectSvcIpc();
	gExitService.wait();
	//test begin
	//pGaugeArrive->printResult();
	//test end
	//pGaugeArrive->disconnectSvcipc();

	GaugeArrival::DeleteInstance();
	return EXIT_SUCCESS;
}
