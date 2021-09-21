; ModuleID = 'sigma.c'
source_filename = "sigma.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%union.pthread_attr_t = type { i64, [48 x i8] }

@.str = private unnamed_addr constant [2 x i8] c"0\00", align 1
@.str.1 = private unnamed_addr constant [8 x i8] c"sigma.c\00", align 1
@__PRETTY_FUNCTION__.reach_error = private unnamed_addr constant [19 x i8] c"void reach_error()\00", align 1
@SIGMA = dso_local constant i32 16, align 4
@array_index = dso_local global i32 -1, align 4
@array = dso_local global i32* null, align 8

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @assume_abort_if_not(i32 %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %6, label %5

5:                                                ; preds = %1
  call void @abort() #5
  unreachable

6:                                                ; preds = %1
  ret void
}

; Function Attrs: noreturn
declare dso_local void @abort() #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @reach_error() #0 {
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.1, i64 0, i64 0), i32 15, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @__PRETTY_FUNCTION__.reach_error, i64 0, i64 0)) #6
  unreachable
}

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__VERIFIER_assert(i32 %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %7, label %5

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %5
  call void @reach_error()
  call void @abort() #6
  unreachable

7:                                                ; preds = %1
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = load i32*, i32** @array, align 8
  %4 = load i32, i32* @array_index, align 4
  %5 = sext i32 %4 to i64
  %6 = getelementptr inbounds i32, i32* %3, i64 %5
  store i32 1, i32* %6, align 4
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i64*, align 8
  store i32 0, i32* %1, align 4
  %5 = call noalias i8* @malloc(i64 128) #7
  %6 = bitcast i8* %5 to i64*
  store i64* %6, i64** %4, align 8
  %7 = call noalias i8* @malloc(i64 64) #7
  %8 = bitcast i8* %7 to i32*
  store i32* %8, i32** @array, align 8
  %9 = load i64*, i64** %4, align 8
  %10 = ptrtoint i64* %9 to i32
  call void @assume_abort_if_not(i32 %10)
  %11 = load i32*, i32** @array, align 8
  %12 = ptrtoint i32* %11 to i32
  call void @assume_abort_if_not(i32 %12)
  store i32 0, i32* %2, align 4
  br label %13

13:                                               ; preds = %24, %0
  %14 = load i32, i32* %2, align 4
  %15 = icmp slt i32 %14, 16
  br i1 %15, label %16, label %27

16:                                               ; preds = %13
  %17 = load i32, i32* @array_index, align 4
  %18 = add nsw i32 %17, 1
  store i32 %18, i32* @array_index, align 4
  %19 = load i64*, i64** %4, align 8
  %20 = load i32, i32* %2, align 4
  %21 = sext i32 %20 to i64
  %22 = getelementptr inbounds i64, i64* %19, i64 %21
  %23 = call i32 @pthread_create(i64* %22, %union.pthread_attr_t* null, i8* (i8*)* @thread, i8* null) #7
  br label %24

24:                                               ; preds = %16
  %25 = load i32, i32* %2, align 4
  %26 = add nsw i32 %25, 1
  store i32 %26, i32* %2, align 4
  br label %13

27:                                               ; preds = %13
  store i32 0, i32* %2, align 4
  br label %28

28:                                               ; preds = %38, %27
  %29 = load i32, i32* %2, align 4
  %30 = icmp slt i32 %29, 16
  br i1 %30, label %31, label %41

31:                                               ; preds = %28
  %32 = load i64*, i64** %4, align 8
  %33 = load i32, i32* %2, align 4
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds i64, i64* %32, i64 %34
  %36 = load i64, i64* %35, align 8
  %37 = call i32 @pthread_join(i64 %36, i8** null)
  br label %38

38:                                               ; preds = %31
  %39 = load i32, i32* %2, align 4
  %40 = add nsw i32 %39, 1
  store i32 %40, i32* %2, align 4
  br label %28

41:                                               ; preds = %28
  store i32 0, i32* %3, align 4
  store i32 0, i32* %2, align 4
  br label %42

42:                                               ; preds = %53, %41
  %43 = load i32, i32* %2, align 4
  %44 = icmp slt i32 %43, 16
  br i1 %44, label %45, label %56

45:                                               ; preds = %42
  %46 = load i32*, i32** @array, align 8
  %47 = load i32, i32* %2, align 4
  %48 = sext i32 %47 to i64
  %49 = getelementptr inbounds i32, i32* %46, i64 %48
  %50 = load i32, i32* %49, align 4
  %51 = load i32, i32* %3, align 4
  %52 = add nsw i32 %51, %50
  store i32 %52, i32* %3, align 4
  br label %53

53:                                               ; preds = %45
  %54 = load i32, i32* %2, align 4
  %55 = add nsw i32 %54, 1
  store i32 %55, i32* %2, align 4
  br label %42

56:                                               ; preds = %42
  %57 = load i32, i32* %3, align 4
  %58 = icmp eq i32 %57, 16
  %59 = zext i1 %58 to i32
  call void @__VERIFIER_assert(i32 %59)
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #3

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #3

declare dso_local i32 @pthread_join(i64, i8**) #4

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noreturn }
attributes #6 = { noreturn nounwind }
attributes #7 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1 (https://github.com/llvm/llvm-project.git 43ff75f2c3feef64f9d73328230d34dac8832a91)"}
