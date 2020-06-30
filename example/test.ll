; ModuleID = 'test.cpp'
source_filename = "test.cpp"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.26.28806"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @"?foo@@YAHH@Z"(i32 %y) #0 {
entry:
  %retval = alloca i32, align 4
  %y.addr = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 %y, i32* %y.addr, align 4
  store i32 0, i32* %x, align 4
  %0 = load i32, i32* %y.addr, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i32 0, i32* %retval, align 4
  br label %return

if.end:                                           ; preds = %entry
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end
  %1 = load i32, i32* %x, align 4
  %2 = load i32, i32* %y.addr, align 4
  %cmp1 = icmp slt i32 %1, %2
  br i1 %cmp1, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %3 = load i32, i32* %x, align 4
  %inc = add nsw i32 %3, 1
  store i32 %inc, i32* %x, align 4
  %4 = load i32, i32* %y.addr, align 4
  %sub = sub nsw i32 %4, 2
  store i32 %sub, i32* %y.addr, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %5 = load i32, i32* %x, align 4
  %6 = load i32, i32* %y.addr, align 4
  %mul = mul nsw i32 %5, %6
  store i32 %mul, i32* %retval, align 4
  br label %return

return:                                           ; preds = %while.end, %if.then
  %7 = load i32, i32* %retval, align 4
  ret i32 %7
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 10.0.0 "}
