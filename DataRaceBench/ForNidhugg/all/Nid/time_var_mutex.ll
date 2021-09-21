; ModuleID = 'time_var_mutex.c'
source_filename = "time_var_mutex.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_mutexattr_t = type { i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

@.str = private unnamed_addr constant [2 x i8] c"0\00", align 1
@.str.1 = private unnamed_addr constant [17 x i8] c"time_var_mutex.c\00", align 1
@__PRETTY_FUNCTION__.reach_error = private unnamed_addr constant [19 x i8] c"void reach_error()\00", align 1
@m_inode = dso_local global %union.pthread_mutex_t zeroinitializer, align 8
@inode = dso_local global i32 0, align 4
@m_busy = dso_local global %union.pthread_mutex_t zeroinitializer, align 8
@busy = dso_local global i32 0, align 4
@block = dso_local global i32 0, align 4

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
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([17 x i8], [17 x i8]* @.str.1, i64 0, i64 0), i32 7, i8* getelementptr inbounds ([19 x i8], [19 x i8]* @__PRETTY_FUNCTION__.reach_error, i64 0, i64 0)) #6
  unreachable
}

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @allocator(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m_inode) #7
  %4 = load i32, i32* @inode, align 4
  %5 = icmp eq i32 %4, 0
  br i1 %5, label %6, label %9

6:                                                ; preds = %1
  %7 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m_busy) #7
  store i32 1, i32* @busy, align 4
  %8 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m_busy) #7
  store i32 1, i32* @inode, align 4
  br label %9

9:                                                ; preds = %6, %1
  store i32 1, i32* @block, align 4
  %10 = load i32, i32* @block, align 4
  %11 = icmp eq i32 %10, 1
  br i1 %11, label %14, label %12

12:                                               ; preds = %9
  br label %13

13:                                               ; preds = %12
  call void @reach_error()
  br label %14

14:                                               ; preds = %13, %9
  %15 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m_inode) #7
  ret i8* null
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_lock(%union.pthread_mutex_t*) #3

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_unlock(%union.pthread_mutex_t*) #3

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @de_allocator(i8* %0) #0 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m_busy) #7
  %4 = load i32, i32* @busy, align 4
  %5 = icmp eq i32 %4, 0
  br i1 %5, label %6, label %12

6:                                                ; preds = %1
  store i32 0, i32* @block, align 4
  %7 = load i32, i32* @block, align 4
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %11, label %9

9:                                                ; preds = %6
  br label %10

10:                                               ; preds = %9
  call void @reach_error()
  br label %11

11:                                               ; preds = %10, %6
  br label %12

12:                                               ; preds = %11, %1
  %13 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m_busy) #7
  ret i8* null
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  %3 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  %4 = load i32, i32* @inode, align 4
  %5 = load i32, i32* @busy, align 4
  %6 = icmp eq i32 %4, %5
  %7 = zext i1 %6 to i32
  call void @assume_abort_if_not(i32 %7)
  %8 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* @m_inode, %union.pthread_mutexattr_t* null) #7
  %9 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* @m_busy, %union.pthread_mutexattr_t* null) #7
  %10 = call i32 @pthread_create(i64* %2, %union.pthread_attr_t* null, i8* (i8*)* @allocator, i8* null) #7
  %11 = call i32 @pthread_create(i64* %3, %union.pthread_attr_t* null, i8* (i8*)* @de_allocator, i8* null) #7
  %12 = load i64, i64* %2, align 8
  %13 = call i32 @pthread_join(i64 %12, i8** null)
  %14 = load i64, i64* %3, align 8
  %15 = call i32 @pthread_join(i64 %14, i8** null)
  %16 = call i32 @pthread_mutex_destroy(%union.pthread_mutex_t* @m_inode) #7
  %17 = call i32 @pthread_mutex_destroy(%union.pthread_mutex_t* @m_busy) #7
  ret i32 0
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_init(%union.pthread_mutex_t*, %union.pthread_mutexattr_t*) #3

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #3

declare dso_local i32 @pthread_join(i64, i8**) #4

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_destroy(%union.pthread_mutex_t*) #3

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
