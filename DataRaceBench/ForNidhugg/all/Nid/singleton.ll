; ModuleID = 'singleton.c'
source_filename = "singleton.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_attr_t = type { i64, [48 x i8] }
%union.pthread_mutexattr_t = type { i32 }

@v = dso_local global i8* null, align 8
@m = dso_local global %union.pthread_mutex_t zeroinitializer, align 8
@.str = private unnamed_addr constant [12 x i8] c"v[0] == 'X'\00", align 1
@.str.1 = private unnamed_addr constant [12 x i8] c"singleton.c\00", align 1
@__PRETTY_FUNCTION__.main = private unnamed_addr constant [15 x i8] c"int main(void)\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread1(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call noalias i8* @malloc(i64 1) #4
  store i8* %3, i8** @v, align 8
  ret i8* null
}

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread2(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m) #4
  %4 = load i8*, i8** @v, align 8
  %5 = getelementptr inbounds i8, i8* %4, i64 0
  store i8 88, i8* %5, align 1
  %6 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m) #4
  ret i8* null
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_lock(%union.pthread_mutex_t*) #1

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_unlock(%union.pthread_mutex_t*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread3(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m) #4
  %4 = load i8*, i8** @v, align 8
  %5 = getelementptr inbounds i8, i8* %4, i64 0
  store i8 89, i8* %5, align 1
  %6 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m) #4
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @thread0(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  store i8* %0, i8** %2, align 8
  %8 = call i32 @pthread_create(i64* %3, %union.pthread_attr_t* null, i8* (i8*)* @thread1, i8* null) #4
  %9 = load i64, i64* %3, align 8
  %10 = call i32 @pthread_join(i64 %9, i8** null)
  %11 = call i32 @pthread_create(i64* %4, %union.pthread_attr_t* null, i8* (i8*)* @thread2, i8* null) #4
  %12 = call i32 @pthread_create(i64* %5, %union.pthread_attr_t* null, i8* (i8*)* @thread3, i8* null) #4
  %13 = call i32 @pthread_create(i64* %6, %union.pthread_attr_t* null, i8* (i8*)* @thread2, i8* null) #4
  %14 = call i32 @pthread_create(i64* %7, %union.pthread_attr_t* null, i8* (i8*)* @thread2, i8* null) #4
  %15 = load i64, i64* %4, align 8
  %16 = call i32 @pthread_join(i64 %15, i8** null)
  %17 = load i64, i64* %5, align 8
  %18 = call i32 @pthread_join(i64 %17, i8** null)
  %19 = load i64, i64* %6, align 8
  %20 = call i32 @pthread_join(i64 %19, i8** null)
  %21 = load i64, i64* %7, align 8
  %22 = call i32 @pthread_join(i64 %21, i8** null)
  ret i8* null
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #1

declare dso_local i32 @pthread_join(i64, i8**) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %3 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* @m, %union.pthread_mutexattr_t* null) #4
  %4 = call i32 @pthread_create(i64* %2, %union.pthread_attr_t* null, i8* (i8*)* @thread0, i8* null) #4
  %5 = load i64, i64* %2, align 8
  %6 = call i32 @pthread_join(i64 %5, i8** null)
  %7 = load i8*, i8** @v, align 8
  %8 = getelementptr inbounds i8, i8* %7, i64 0
  %9 = load i8, i8* %8, align 1
  %10 = sext i8 %9 to i32
  %11 = icmp eq i32 %10, 88
  br i1 %11, label %12, label %13

12:                                               ; preds = %0
  br label %14

13:                                               ; preds = %0
  call void @__assert_fail(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.1, i64 0, i64 0), i32 76, i8* getelementptr inbounds ([15 x i8], [15 x i8]* @__PRETTY_FUNCTION__.main, i64 0, i64 0)) #5
  unreachable

14:                                               ; preds = %12
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_init(%union.pthread_mutex_t*, %union.pthread_mutexattr_t*) #1

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #3

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }
attributes #5 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1 (https://github.com/llvm/llvm-project.git 43ff75f2c3feef64f9d73328230d34dac8832a91)"}
