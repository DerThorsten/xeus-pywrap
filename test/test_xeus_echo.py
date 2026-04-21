

import tempfile
import unittest
import jupyter_kernel_test


class EchoKernelTests(jupyter_kernel_test.KernelTests):

    kernel_name = "xeus_echo"
    language_name = "echo"
    code_hello_world = "hello, world"


    def test_stdout(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code="hello, world")
        self.assertEqual(output_msgs[0]["msg_type"], "stream")
        self.assertEqual(output_msgs[0]["content"]["name"], "stdout")
        self.assertEqual(output_msgs[0]["content"]["text"], "hello, world")