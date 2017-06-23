#include <coroutine/engine.h>

#include <iostream>
#include <string.h>
#include <setjmp.h>

namespace Coroutine {

void Engine::Store(context& ctx) {
    // TODO: implements
	char stack_top; 
    //std::cout << &ctx<<" 0\n";

    ctx.Low = StackBottom;
    ctx.Hight = &stack_top;
    //std::cout << "1\n";
	/* проверяем доступное нам пространство в стеке */

    if (ctx.Hight < ctx.Low){
    	char *tmp = ctx.Hight;
    	ctx.Hight  = ctx.Low;
    	ctx.Low = tmp;
    }
    //std::cout << "2\n";

    int size = ctx.Hight - ctx.Low;

    /* сохраняем стек */
    char *buf = std::get<0>(ctx.Stack);
    int old_size = std::get<1>(ctx.Stack);
    //std::cout << "3\n";
    if (old_size < size || buf == nullptr){	
    	//std::cout << "4\n";

    	if (buf != nullptr) {
  		    //std::cout << buf<< " 5\n";

			delete[] buf;
		}
   		 //std::cout << size << " 6\n";

		buf = new char[size];
	}
	    //std::cout << "7\n";

	memcpy(buf, ctx.Low, size);

	std::get<0>(ctx.Stack) = buf;    
	std::get<1>(ctx.Stack) = size;
}

void Engine::Restore(context& ctx) {
    // TODO: implements
    char stack_start;
    /* идём до самого низу: восстанавливаем стек */
    if (ctx.Low <= &stack_start && &stack_start <= ctx.Hight){
        Restore(ctx);  
    }

	memcpy(ctx.Low, std::get<0>(ctx.Stack), std::get<1>(ctx.Stack));
	longjmp(ctx.Environment, 1);
	
}

void Engine::Enter(context& ctx) {
    // TODO: implements
}

void Engine::yield() {
    // TODO: implements
		if(cur_routine == nullptr)
			sched(nullptr);
		else 
			sched(cur_routine -> caller);
	}


void Engine::sched(void *routine) {
    // TODO: implements
    // setjmp, longjmp...
	
	context *ctx = (context *) routine; // new context to schedule
	

	if (routine == nullptr) {
		//std::cout << cur_routine << "  FUCK\n" ; 
		
		if (cur_routine != nullptr)
			Store(*cur_routine);
//		std::cout << "lonjmp" << std::endl;
		longjmp(StartEnvironment ,1);
	}
	else {
		if (cur_routine != nullptr) {  // cur_routine does not exist until the first sched() call
			if (setjmp(cur_routine->Environment) == 0)// && (setjmp(StartEnvironment) == 0))
			{ // save environment
				Store(*cur_routine); // save stack
			
				if (cur_routine->caller == ctx) { // to avoid calling cycles
					cur_routine->caller = nullptr;
				} else {
					ctx->caller = cur_routine;
				}
				  
				cur_routine->callee = ctx;
			} 
			else {
				return;
			}
		} 

		cur_routine = ctx;
		Restore(*ctx);
		}
	}



} // namespace Coroutine
