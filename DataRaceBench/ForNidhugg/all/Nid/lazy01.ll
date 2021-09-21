; ModuleID = 'lazy01.c'
source_filename = "lazy01.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_mutexattr_t = type { i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

@.str = private unnamed_addr constant [2 x i8] c"0\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"lazy01.c\00", align 1
@__PRETTY_FUNCTION__.reach_error = private unnamed_addr constant [19 x i8] c"void reach_error()\00", align 1
@data = dso_local global i32 0, align 4
@mutex = dso_local global %union.pthread_mutex_t zeroinitializer, align 8

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @reach_error() #0 {
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i64 0, i64 0), i32 11, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @__PRETTY_FUNCTION__.reach_error, i64 0, i64 0)) #5
  unreachable
}

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread1(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @mutex) #6
  %4 = load i32, i32* @data, align 4
  %5 = add nsw i32 %4, 1
  store i32 %5, i32* @data, align 4
  %6 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @mutex) #6
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
  %3 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @mutex) #6
  %4 = load i32, i32* @data, align 4
  %5 = add nsw i32 %4, 2
  store i32 %5, i32* @data, align 4
  %6 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @mutex) #6
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread3(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @mutex) #6
  %4 = load i32, i32* @data, align 4
  %5 = icmp sge i32 %4, 3
  br i1 %5, label %6, label %8

6:                                                ; preds = %1
  br label %7

7:                                                ; preds = %6
  call void @reach_error()
  call void @abort() #7
  unreachable

8:                                                ; preds = %1
  %9 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @mutex) #6
  ret i8* null
}

; Function Attrs: noreturn
declare dso_local void @abort() #3

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %5 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* @mutex, %union.pthread_mutexattr_t* null) #6
  %6 = call i32 @pthread_create(i64* %2, %union.pthread_attr_t* null, i8* (i8*)* @thread1, i8* null) #6
  %7 = call i32 @pthread_create(i64* %3, %union.pthread_attr_t* null, i8* (i8*)* @thread2, i8* null) #6
  %8 = call i32 @pthread_create(i64* %4, %union.pthread_attr_t* null, i8* (i8*)* @thread3, i8* null) #6
  %9 = load i64, i64* %2, align 8
  %10 = call i32 @pthread_join(i64 %9, i8** null)
  %11 = load i64, i64* %3, align 8
  %12 = call i32 @pthread_join(i64 %11, i8** null)
  %13 = load i64, i64* %4, align 8
  %14 = call i32 @pthread_join(i64 %13, i8** null)
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_init(%union.pthread_mutex_t*, %union.pthread_mutexattr_t*) #2

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #2

declare dso_local i32 @pthread_join(i64, i8**) #4

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noreturn nounwind }
attributes #6 = { nounwind }
attributes #7 = { noreturn }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1 (https://github.com/llvm/llvm-project.git 43ff75f2c3feef64f9d73328230d34dac8832a91)"}
