; ModuleID = 'stateful01-2.c'
source_filename = "stateful01-2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_mutexattr_t = type { i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

@.str = private unnamed_addr constant [2 x i8] c"0\00", align 1
@.str.1 = private unnamed_addr constant [15 x i8] c"stateful01-2.c\00", align 1
@__PRETTY_FUNCTION__.reach_error = private unnamed_addr constant [19 x i8] c"void reach_error()\00", align 1
@ma = dso_local global %union.pthread_mutex_t zeroinitializer, align 8
@data1 = dso_local global i32 0, align 4
@data2 = dso_local global i32 0, align 4
@mb = dso_local global %union.pthread_mutex_t zeroinitializer, align 8

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @reach_error() #0 {
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str.1, i64 0, i64 0), i32 11, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @__PRETTY_FUNCTION__.reach_error, i64 0, i64 0)) #5
  unreachable
}

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread1(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @ma) #6
  %4 = load i32, i32* @data1, align 4
  %5 = add nsw i32 %4, 1
  store i32 %5, i32* @data1, align 4
  %6 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @ma) #6
  %7 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @ma) #6
  %8 = load i32, i32* @data2, align 4
  %9 = add nsw i32 %8, 1
  store i32 %9, i32* @data2, align 4
  %10 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @ma) #6
  ret i8* null
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_lock(%union.pthread_mutex_t*) #2

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_unlock(%union.pthread_mutex_t*) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread2(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @ma) #6
  %4 = load i32, i32* @data1, align 4
  %5 = add nsw i32 %4, 5
  store i32 %5, i32* @data1, align 4
  %6 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @ma) #6
  %7 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @ma) #6
  %8 = load i32, i32* @data2, align 4
  %9 = sub nsw i32 %8, 6
  store i32 %9, i32* @data2, align 4
  %10 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @ma) #6
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %4 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* @ma, %union.pthread_mutexattr_t* null) #6
  %5 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* @mb, %union.pthread_mutexattr_t* null) #6
  store i32 10, i32* @data1, align 4
  store i32 10, i32* @data2, align 4
  %6 = call i32 @pthread_create(i64* %2, %union.pthread_attr_t* null, i8* (i8*)* @thread1, i8* null) #6
  %7 = call i32 @pthread_create(i64* %3, %union.pthread_attr_t* null, i8* (i8*)* @thread2, i8* null) #6
  %8 = load i64, i64* %2, align 8
  %9 = call i32 @pthread_join(i64 %8, i8** null)
  %10 = load i64, i64* %3, align 8
  %11 = call i32 @pthread_join(i64 %10, i8** null)
  %12 = load i32, i32* @data1, align 4
  %13 = icmp ne i32 %12, 16
  br i1 %13, label %14, label %19

14:                                               ; preds = %0
  %15 = load i32, i32* @data2, align 4
  %16 = icmp ne i32 %15, 5
  br i1 %16, label %17, label %19

17:                                               ; preds = %14
  br label %18

18:                                               ; preds = %17
  call void @reach_error()
  call void @abort() #7
  unreachable

19:                                               ; preds = %14, %0
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_init(%union.pthread_mutex_t*, %union.pthread_mutexattr_t*) #2

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #2

declare dso_local i32 @pthread_join(i64, i8**) #3

; Function Attrs: noreturn
declare dso_local void @abort() #4

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noreturn nounwind }
attributes #6 = { nounwind }
attributes #7 = { noreturn }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1 (https://github.com/llvm/llvm-project.git 43ff75f2c3feef64f9d73328230d34dac8832a91)"}
