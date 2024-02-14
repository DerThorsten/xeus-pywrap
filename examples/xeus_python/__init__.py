import xpywrap
import asyncio
import io
import traceback

from .async_exec import exec_with_return

import builtins as __builtin__



_interpreter = [None]

xeus_pywrap_python_config = {
    "use_cell_lock": True
}


def print(*args, **kwargs):
    interpreter_instance = _interpreter[0]
    if interpreter_instance is not None:

        output = io.StringIO()
        __builtin__.print(*args, file=output, **kwargs)
        contents = output.getvalue()
        output.close()
        interpreter_instance.publish_stdout(contents)

    else:
        # Adding new arguments to the print function signature 
        # is probably a bad idea.
        # Instead consider testing if custom argument keywords
        # are present in kwargs
        __builtin__.print('My overridden print() function!')
        return __builtin__.print(*args, **kwargs)


class Interpreter(xpywrap.InterpreterBase):
    def __init__(self):
        _interpreter[0] = self
        
        self.cell_lock = asyncio.Lock()



    def kernel_info_request(self):
        # get python version
        return xpywrap.kernel_info_response(
            implementation="xpython",
            language_info=xpywrap.language_info_response(
                name="python",
                version="3.12.0",
                mimetype="text/x-python",
                file_extension=".py"
            )
        )
    
    def execute_request(self, execution_count, code, **kwargs):
        
        async def task():
            # async with self.cell_lock:
                try:
                    ret = await exec_with_return(code, globals(), locals())
                except Exception as e:

                    # convert traceback to a list of strings
                    tb = traceback.format_exception(type(e), e, e.__traceback__)

                    __builtin__.print("rb",tb)

                    self.publish_execution_error(
                        ename=type(e).__name__,
                        evalue=str(e),
                        trace_back=[],
                    )
                    return
                result_data =  {"text/plain": ""}
                if ret is not None:
                    result_data =  {"text/plain": repr(ret)}
                self.publish_execution_result(execution_count, result_data)

        if xeus_pywrap_python_config["use_cell_lock"]:
            async def wrapped_task():
                async with self.cell_lock:
                    await task()
        else:
            wrapped_task = task
        asyncio.create_task(wrapped_task())


        return xpywrap.execute_ok_response(execution_count)



def make_interpreter(*args, **kwargs):
    print("make_interpreter", args, kwargs)
    return Interpreter()


