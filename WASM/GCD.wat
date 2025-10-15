(module

  (func $fun0 (param $var0 i32) (param $var1 i32) (result i32)
    (local $var2 i32) ;; declaration of 'temp'

    (block $while_block1
      (loop $while_loop1
        (local.get $var1) ;; Put 'b' on the stack.
        (i32.const 0)     ;; Put a zero on the stack.
        (i32.ne)          ;; Result of condition.

        (i32.const 0)
        (i32.eq)
        (br_if $while_block1)

        (local.get $var1)
        (local.set $var2)

        (local.get $var0) ;; Put a on stack
        (local.get $var1) ;; Put b on stack
        (i32.rem_s)
        (local.set $var1)

        (local.get $var2)
        (local.set $var0)

        (br $while_loop1)
      )
    )

    (local.get $var0)
    (return)
  )

  (export "GCD" (func $fun0))
)