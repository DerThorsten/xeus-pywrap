import xeus_python_wrapper_kernel as xpwk


from .server import *
import tempfile
import json

# this dir
import os
from pathlib import Path
import sys

import playwright
from playwright.sync_api import Page, expect, sync_playwright

# add bld dir to path
THIS_DIR = Path(os.path.dirname(os.path.realpath(__file__)))
SERVE_DIR = THIS_DIR  / 'serve_dir'



class Interpreter(xpwk.InterpreterBase):
    def __init__(self):
        super().__init__()
        self.execution_count = 0


    def init_server(self):
        self.serve_dir = SERVE_DIR
        self.server_port = find_free_port()
        self.server = None
        self.server_thread = None
        s,t = start_server(work_dir=self.serve_dir.name, port=self.server_port)
        self.page_url = f"http://127.0.0.1:{self.server_port}/index.html"
        self.server = s
        self.server_thread = t

    def start_page(self):
        

        self.p =  sync_playwright().start()
        self.browser = self.p.chromium.launch(headless=True)
        self.page = self.browser.new_page()
        self.page.goto(self.page_url)

        async def handle_console(msg):
            txt = str(msg)
            print(txt)
            self.publish_stream('stdout', txt)
            # xpwk.publish_stdout_stream(txt)
            print('published')

        self.page.on("console", handle_console)

    def shutdown_request(self):
        self.server.shutdown()
        self.server_thread.join()

    

    def configure(self):
        print('configure')
           
        self.init_server()
        self.start_page()
         
    def execute_request(self, execution_counter, code, silent, store_history,
                user_expressions, allow_stdin):
        print('execute_request')

        status = self.page.evaluate(f"""async () => {{
            try {{
                {code}
            }}  catch (e) {{
                return {{
                    error_message: e.message,
                    has_error: true
                }}
            }}
            return {{
                has_error: false
            }}
        }}""")


        print(status)
        if status['has_error']:
            
            if not silent:
                # xpwk.publish_stderr_stream(status["error_message"])
                self.publish_execution_error("some error", 
                    status["error_message"],[])
            
            return {'status': 'error',
                    'execution_count': self.execution_count,
                    'ename': status["error_message"],
                    'evalue': status["error_message"],
                    'traceback': []}


        if not silent:
            self.publish_execution_result(self.execution_count,
                    {},{})


        return {'status': 'ok',
                'execution_count': self.execution_count,
                'payload': [],
                'user_expressions': {}}

    def kernel_info_request(self):
        print('kernel_info_request')
        kernel_res = {}
        kernel_res["status"] = "ok"
        kernel_res["protocol_version"] = "5.3"
        kernel_res["implementation"] = "xjavascript"
        kernel_res["implementation_version"] = "0.1.0"
        kernel_res["language_info"] = {}
        kernel_res["language_info"]["name"] = "javascript"
        kernel_res["language_info"]["version"] = "ES6"
        kernel_res["language_info"]["mimetype"] = "application/javascript"
        kernel_res["language_info"]["file_extension"] = ".js"
        kernel_res["language_info"]["pygments_lexer"] = ""
        kernel_res["language_info"]["codemirror_mode"] = ""
        kernel_res["language_info"]["nbconvert_exporter"] = ""
        kernel_res["banner"] = "xjavascript kernel"
        kernel_res["debugger"] = False
        kernel_res["help_links"] = ["a"]
    
        
        return kernel_res

if __name__ == '__main__':

    # try to get connection file from command line
    connection_filename = None
    if len(sys.argv) > 1:
        connection_filename = sys.argv[1]
  

    interpreter = MyInterpreter()
    kernel = xpwk.kernel(interpreter=interpreter, connection_filename=connection_filename)

    # this will block
    kernel.start()

    print("bye")
    