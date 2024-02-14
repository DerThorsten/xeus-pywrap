import xpywrap


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



class Interpreter(xpywrap.InterpreterBase):
    def __init__(self):
        super().__init__()


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
            self.publish_stdout(msg)

        self.page.on("console", handle_console)

    def shutdown_request(self):
        self.server.shutdown()
        self.server_thread.join()
        return xpywrap.shutdown_response()

    

    def configure(self):
           
        self.init_server()
        self.start_page()
         
    def execute_request(self, code, execution_count, silent, **kwargs):
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

        if status['has_error']:
            
            if not silent:
                self.publish_execution_error("some error", status["error_message"])
            
            return xpywrap.execute_error_repsonse(
                execution_count=execution_count,
                ename = status["error_message"],
                evalue = status["error_message"]
            )

        if not silent:
            self.publish_execution_result(execution_count)

        return xpywrap.execute_ok_response(execution_count)

    def kernel_info_request(self):

        return xpywrap.kernel_info_response(
            implementation="xjavascript",
            language_info=xpywrap.language_info_response(
                name="javascript",
                version="ES6",
                mimetype="application/javascript",
                file_extension=".js"
            )
        )



def make_interpreter(*args, **kwargs):
    return Interpreter()