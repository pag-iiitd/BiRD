; ModuleID = 'bankacct.c'
source_filename = "bankacct.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_mutexattr_t = type { i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

@lock = internal global %union.pthread_mutex_t zeroinitializer, align 8
@g_account = internal global i32 0, align 4
@.str = private unnamed_addr constant [6 x i8] c"x:y:h\00", align 1
@optarg = external dso_local global i8*, align 8
@.str.1 = private unnamed_addr constant [15 x i8] c"invalid option\00", align 1
@.str.2 = private unnamed_addr constant [22 x i8] c"pthread_create failed\00", align 1
@.str.3 = private unnamed_addr constant [15 x i8] c"g_account == 9\00", align 1
@.str.4 = private unnamed_addr constant [11 x i8] c"bankacct.c\00", align 1
@__PRETTY_FUNCTION__.main = private unnamed_addr constant [23 x i8] c"int main(int, char **)\00", align 1
@.str.5 = private unnamed_addr constant [66 x i8] c"%s -x <x> -y <y>\0A-x : input for thread 1\0A-y : input for thread 2\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @Deposit(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i8* %0, i8** %2, align 8
  %5 = ptrtoint i8** %2 to i32
  store i32 %5, i32* %3, align 4
  %6 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @lock) #5
  %7 = load volatile i32, i32* @g_account, align 4
  store i32 %7, i32* %4, align 4
  %8 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @lock) #5
  %9 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @lock) #5
  %10 = load i32, i32* %4, align 4
  %11 = load i32, i32* %3, align 4
  %12 = add nsw i32 %10, %11
  store volatile i32 %12, i32* @g_account, align 4
  %13 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @lock) #5
  ret i8* null
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_lock(%union.pthread_mutex_t*) #1

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_unlock(%union.pthread_mutex_t*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @Withdraw(i32 %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %4 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @lock) #5
  %5 = load volatile i32, i32* @g_account, align 4
  store i32 %5, i32* %3, align 4
  %6 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @lock) #5
  %7 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @lock) #5
  %8 = load i32, i32* %3, align 4
  %9 = load i32, i32* %2, align 4
  %10 = sub nsw i32 %8, %9
  store volatile i32 %10, i32* @g_account, align 4
  %11 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @lock) #5
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main(i32 %0, i8** %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca [10 x i64], align 16
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  store i32 13, i32* %9, align 4
  store i32 0, i32* %10, align 4
  br label %11

11:                                               ; preds = %29, %2
  %12 = load i32, i32* %4, align 4
  %13 = load i8**, i8*** %5, align 8
  %14 = call i32 @getopt(i32 %12, i8** %13, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str, i64 0, i64 0)) #5
  store i32 %14, i32* %7, align 4
  %15 = icmp ne i32 %14, -1
  br i1 %15, label %16, label %30

16:                                               ; preds = %11
  %17 = load i32, i32* %7, align 4
  switch i32 %17, label %28 [
    i32 104, label %18
    i32 120, label %20
    i32 121, label %24
  ]

18:                                               ; preds = %16
  %19 = load i8**, i8*** %5, align 8
  call void @usage(i8** %19)
  store i32 0, i32* %3, align 4
  br label %47

20:                                               ; preds = %16
  %21 = load i8*, i8** @optarg, align 8
  %22 = call i64 @strtol(i8* %21, i8** null, i32 0) #5
  %23 = trunc i64 %22 to i32
  store i32 %23, i32* %9, align 4
  br label %29

24:                                               ; preds = %16
  %25 = load i8*, i8** @optarg, align 8
  %26 = call i64 @strtol(i8* %25, i8** null, i32 0) #5
  %27 = trunc i64 %26 to i32
  store i32 %27, i32* %10, align 4
  br label %29

28:                                               ; preds = %16
  call void (i32, i8*, ...) @errx(i32 1, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str.1, i64 0, i64 0)) #6
  unreachable

29:                                               ; preds = %24, %20
  br label %11

30:                                               ; preds = %11
  %31 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* @lock, %union.pthread_mutexattr_t* null) #5
  store volatile i32 10, i32* @g_account, align 4
  %32 = getelementptr inbounds [10 x i64], [10 x i64]* %6, i64 0, i64 0
  %33 = call i32 @pthread_create(i64* %32, %union.pthread_attr_t* null, i8* (i8*)* @Deposit, i8* inttoptr (i64 1 to i8*)) #5
  store i32 %33, i32* %8, align 4
  %34 = load i32, i32* %8, align 4
  %35 = icmp ne i32 %34, 0
  br i1 %35, label %36, label %37

36:                                               ; preds = %30
  call void (i32, i8*, ...) @err(i32 1, i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.2, i64 0, i64 0)) #6
  unreachable

37:                                               ; preds = %30
  %38 = call i8* @Withdraw(i32 2)
  %39 = getelementptr inbounds [10 x i64], [10 x i64]* %6, i64 0, i64 0
  %40 = load i64, i64* %39, align 16
  %41 = call i32 @pthread_join(i64 %40, i8** null)
  %42 = load volatile i32, i32* @g_account, align 4
  %43 = icmp eq i32 %42, 9
  br i1 %43, label %44, label %45

44:                                               ; preds = %37
  br label %46

45:                                               ; preds = %37
  call void @__assert_fail(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str.3, i64 0, i64 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str.4, i64 0, i64 0), i32 112, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @__PRETTY_FUNCTION__.main, i64 0, i64 0)) #7
  unreachable

46:                                               ; preds = %44
  store i32 0, i32* %3, align 4
  br label %47

47:                                               ; preds = %46, %18
  %48 = load i32, i32* %3, align 4
  ret i32 %48
}

; Function Attrs: nounwind
declare dso_local i32 @getopt(i32, i8**, i8*) #1

; Function Attrs: noinline nounwind optnone uwtable
define internal void @usage(i8** %0) #0 {
  %2 = alloca i8**, align 8
  store i8** %0, i8*** %2, align 8
  %3 = load i8**, i8*** %2, align 8
  %4 = getelementptr inbounds i8*, i8** %3, i64 0
  %5 = load i8*, i8** %4, align 8
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([66 x i8], [66 x i8]* @.str.5, i64 0, i64 0), i8* %5)
  ret void
}

; Function Attrs: nounwind
declare dso_local i64 @strtol(i8*, i8**, i32) #1

; Function Attrs: noreturn
declare dso_local void @errx(i32, i8*, ...) #2

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_init(%union.pthread_mutex_t*, %union.pthread_mutexattr_t*) #1

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #1

; Function Attrs: noreturn
declare dso_local void @err(i32, i8*, ...) #2

declare dso_local i32 @pthread_join(i64, i8**) #3

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #4

declare dso_local i32 @printf(i8*, ...) #3

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind }
attributes #6 = { noreturn }
attributes #7 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1 (https://github.com/llvm/llvm-project.git 43ff75f2c3feef64f9d73328230d34dac8832a91)"}
