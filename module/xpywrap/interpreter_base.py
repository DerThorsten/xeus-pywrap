from abc import ABC, abstractmethod
from contextlib import redirect_stdout, contextmanager







class InterpreterBase(ABC):

    def __init__(self):
        self._interpreter_impl = None
    
    def _set_interpreter_impl(self, interpreter_impl):
        self._interpreter_impl = interpreter_impl


    

    # redirection layer. The  _ prefixed methods
    # are called by the c++ interpreter implementation.
    # And the _ prefixed methods call the user defined
    # functions in a more "pythonic" way and may sanitize
    # input and output.

    def _execute_request(self, execution_count, code, silent, store_history, user_expressions, allow_stdin):
        return self.execute_request(
            execution_count=execution_count,
            code=code,
            silent=silent,
            store_history=store_history,
            user_expressions=user_expressions,
            allow_stdin=allow_stdin
        )
    
    def _kernel_info_request(self):
        return self.kernel_info_request()
    
    def _inspect_request(self, code, cursor_pos, detail_level):
        return self.inspect_request(
            code=code,
            cursor_pos=cursor_pos,
            detail_level=detail_level
        )
    
    def _is_complete_request(self, code):
        return self.is_complete_request(code=code)
    
    def _complete_request(self, code, cursor_pos):
        return self.complete_request(
            code=code,
            cursor_pos=cursor_pos
        )
    
    def _shutdown_request(self, restart):
        return self.shutdown_request(restart=restart)
    
    def _configure(self):
        return self.configure()
    
       









    @abstractmethod
    def execute_request(self, *args, **kwargs):
        raise NotImplementedError

    @abstractmethod
    def kernel_info_request(self, *args, **kwargs):
        raise NotImplementedError

    # things to optionally implement
    def configure(self):
        pass
    
    def inspect_request(self, *args, **kwargs):
        return {"status": "ok", "found": False, "data": {}, "metadata": {}}

    def is_complete_request(self, *args, **kwargs):
        return {"status": "complete", "indent": ""}

    def complete_request(self, *args, **kwargs):
        return {"matches": [], "cursor_start": 0, "cursor_end": 0, "metadata": {}, "status": "ok"}

    def shutdown_request(self, *args, **kwargs):
        return {"status": "ok", "restart": False}
    

    
    # things to call
    def publish_stream(self, name, text):
        self._interpreter_impl.publish_stream(name, text)
    
    def publish_stdout(self, text):
        if not isinstance(text, str):
            text = str(text)
        self._interpreter_impl.publish_stream("stdout", text)
    
    def publish_stderr(self, text):
        if not isinstance(text, str):
            text = str(text)
        self._interpreter_impl.publish_stream("stderr", text)
    
    def display_data(self, data, metadata=None, transient=None):
        if metadata is None:
            metadata = {}
        if transient is None:
            transient = {}
        self._interpreter_impl.display_data(data, metadata, transient)
    
    def update_display_data(self, data, metadata, transient):
        self._interpreter_impl.update_display_data(data, metadata, transient)
    
    def publish_execution_input(self, code, execution_count):
        self._interpreter_impl.publish_execution_input(code, execution_count)
    
    def publish_execution_result(self, execution_count, data=None, metadata=None):
        if data is None:
            data = {}
        if metadata is None:
            metadata = {}
        self._interpreter_impl.publish_execution_result(execution_count, data, metadata)
    
    def publish_execution_error(self, ename, evalue, trace_back=None):
        if trace_back is None:
            trace_back = []
        
        self._interpreter_impl.publish_execution_error(ename, evalue, trace_back)
    


    @contextmanager
    def redirect_output(self, func):
        with redirect_stdout(func):
            yield