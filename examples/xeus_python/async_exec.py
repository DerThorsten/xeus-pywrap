import asyncio
import ast

async def exec_with_return(code: str, globals:dict, locals:dict):
    code = code.rstrip()
    code = code.rstrip("\n")
    last_char = code[-1]
    code_ast = ast.parse(code)
    last_expression = None
    if code_ast.body and last_char != ";":
        if isinstance(a_last := code_ast.body[-1], ast.Expr):
            last_expression = ast.unparse(code_ast.body.pop())
            print("last_expression", last_expression)

    code = compile(
            ast.unparse(code_ast),
            "<string>",
            "exec",
            flags=ast.PyCF_ALLOW_TOP_LEVEL_AWAIT,
        )
    coroutine = eval(code, globals, locals)
    if coroutine is not None:
        await coroutine
 
    if last_expression:
        
        code = compile(
            last_expression,
            "<string>",
            "eval",
            flags=ast.PyCF_ALLOW_TOP_LEVEL_AWAIT,
        )
        ret = eval(code, globals, locals)
        # if ret is a coroutine, await it
        if asyncio.iscoroutine(ret) and last_expression.startswith("await"):
            ret = await ret
        return ret


if __name__ == "__main__":
    code = """
print("Hello");
await asyncio.sleep(1)
"""



    r = asyncio.run(exec_with_return(code, globals(), locals()))
    print("ret", r)