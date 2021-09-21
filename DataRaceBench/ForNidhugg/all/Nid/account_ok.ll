; ModuleID = 'account_ok.c'
source_filename = "account_ok.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_mutexattr_t = type { i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

@deposit_done = dso_local global i8 0, align 1
@withdraw_done = dso_local global i8 0, align 1
@m = dso_local global %union.pthread_mutex_t zeroinitializer, align 8
@balance = dso_local global i32 0, align 4
@y = dso_local global i32 0, align 4
@z = dso_local global i32 0, align 4
@x = dso_local global i32 0, align 4
@.str = private unnamed_addr constant [23 x i8] c"balance == (x + y) - z\00", align 1
@.str.1 = private unnamed_addr constant [13 x i8] c"account_ok.c\00", align 1
@__PRETTY_FUNCTION__.check_result = private unnamed_addr constant [27 x i8] c"void *check_result(void *)\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @deposit(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  store i8* %0, i8** %3, align 8
  %4 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m) #3
  %5 = load i32, i32* @balance, align 4
  %6 = load i32, i32* @y, align 4
  %7 = add nsw i32 %5, %6
  store i32 %7, i32* @balance, align 4
  store i8 1, i8* @deposit_done, align 1
  %8 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m) #3
  %9 = load i8*, i8** %2, align 8
  ret i8* %9
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_lock(%union.pthread_mutex_t*) #1

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_unlock(%union.pthread_mutex_t*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @withdraw(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  store i8* %0, i8** %3, align 8
  %4 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m) #3
  %5 = load i32, i32* @balance, align 4
  %6 = load i32, i32* @z, align 4
  %7 = sub nsw i32 %5, %6
  store i32 %7, i32* @balance, align 4
  store i8 1, i8* @withdraw_done, align 1
  %8 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m) #3
  %9 = load i8*, i8** %2, align 8
  ret i8* %9
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @check_result(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  store i8* %0, i8** %3, align 8
  %4 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m) #3
  %5 = load i8, i8* @deposit_done, align 1
  %6 = trunc i8 %5 to i1
  br i1 %6, label %7, label %21

7:                                                ; preds = %1
  %8 = load i8, i8* @withdraw_done, align 1
  %9 = trunc i8 %8 to i1
  br i1 %9, label %10, label %21

10:                                               ; preds = %7
  %11 = load i32, i32* @balance, align 4
  %12 = load i32, i32* @x, align 4
  %13 = load i32, i32* @y, align 4
  %14 = add nsw i32 %12, %13
  %15 = load i32, i32* @z, align 4
  %16 = sub nsw i32 %14, %15
  %17 = icmp eq i32 %11, %16
  br i1 %17, label %18, label %19

18:                                               ; preds = %10
  br label %20

19:                                               ; preds = %10
  call void @__assert_fail(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str.1, i64 0, i64 0), i32 30, i8* getelementptr inbounds ([27 x i8], [27 x i8]* @__PRETTY_FUNCTION__.check_result, i64 0, i64 0)) #4
  unreachable

20:                                               ; preds = %18
  br label %21

21:                                               ; preds = %20, %7, %1
  %22 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m) #3
  %23 = load i8*, i8** %2, align 8
  ret i8* %23
}

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %5 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* @m, %union.pthread_mutexattr_t* null) #3
  store i32 1, i32* @x, align 4
  store i32 2, i32* @y, align 4
  store i32 4, i32* @z, align 4
  %6 = load i32, i32* @x, align 4
  store i32 %6, i32* @balance, align 4
  %7 = call i32 @pthread_create(i64* %4, %union.pthread_attr_t* null, i8* (i8*)* @check_result, i8* null) #3
  %8 = call i32 @pthread_create(i64* %2, %union.pthread_attr_t* null, i8* (i8*)* @deposit, i8* null) #3
  %9 = call i32 @pthread_create(i64* %3, %union.pthread_attr_t* null, i8* (i8*)* @withdraw, i8* null) #3
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_init(%union.pthread_mutex_t*, %union.pthread_mutexattr_t*) #1

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #1

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }
attributes #4 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1 (https://github.com/llvm/llvm-project.git 43ff75f2c3feef64f9d73328230d34dac8832a91)"}
