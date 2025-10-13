(module
  (func $Add (param $var1 i32) (param $var2 i32) (result i32)
    (i32.add (local.get $var1) (local.get $var2))
  )

  (export "Add" (func $Add))
)