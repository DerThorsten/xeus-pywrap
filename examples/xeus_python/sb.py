import ast
import asyncio




src="""
x = 1
await asyncio.sleep(0.1)
print('done')
x
"""

async def exec_with_return(code: str, globals:dict, locals:dict):
    a = ast.parse(code)
    last_expression = None
    if a.body:
        if isinstance(a_last := a.body[-1], ast.Expr):
            last_expression = ast.unparse(a.body.pop())
        elif isinstance(a_last, ast.Assign):
            last_expression = ast.unparse(a_last.targets[0])
        elif isinstance(a_last, (ast.AnnAssign, ast.AugAssign)):
            last_expression = ast.unparse(a_last.target)

    code = compile(
            ast.unparse(a),
            "<string>",
            "exec",
            flags=ast.PyCF_ALLOW_TOP_LEVEL_AWAIT,
        )
    # awaited exec
    await eval(code, globals, locals)
    


    # exec(ast.unparse(a), globals, locals,flags=ast.PyCF_ALLOW_TOP_LEVEL_AWAIT)
    if last_expression:

        code = compile(
            last_expression,
            "<string>",
            "eval"
        )
        return eval(code, globals, locals)



async  def  main():
    
    code = """
x = 1
await asyncio.sleep(0.1)
print('done')
x
"""
    r = await exec_with_return(code, globals(), locals())
    print(r)
    print("done")



asyncio.run(main())


# async def main() -> None:
#     code = compile(
#         src,
#         "<string>",
#         "eval",
#         flags=ast.PyCF_ALLOW_TOP_LEVEL_AWAIT,
#     )
#     coroutine: Awaitable | None = eval(code)
#     if coroutine is not None:
#         res = await coroutine
#         print(res)

# asyncio.run(main())


