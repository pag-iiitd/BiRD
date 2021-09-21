; ModuleID = 'bluetooth_driver_bad.c'
source_filename = "bluetooth_driver_bad.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%struct.DEVICE = type { i32, i8, i8 }
%union.pthread_mutexattr_t = type { i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

@m = dso_local global %union.pthread_mutex_t zeroinitializer, align 8
@stopped = dso_local global i8 0, align 1
@.str = private unnamed_addr constant [9 x i8] c"!stopped\00", align 1
@.str.1 = private unnamed_addr constant [23 x i8] c"bluetooth_driver_bad.c\00", align 1
@__PRETTY_FUNCTION__.BCSP_PnpAdd = private unnamed_addr constant [37 x i8] c"void BCSP_PnpAdd(DEVICE_EXTENSION *)\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @BCSP_IoIncrement(%struct.DEVICE* %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca %struct.DEVICE*, align 8
  store %struct.DEVICE* %0, %struct.DEVICE** %3, align 8
  %4 = load %struct.DEVICE*, %struct.DEVICE** %3, align 8
  %5 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %4, i32 0, i32 1
  %6 = load i8, i8* %5, align 4
  %7 = trunc i8 %6 to i1
  br i1 %7, label %8, label %9

8:                                                ; preds = %1
  store i32 -1, i32* %2, align 4
  br label %18

9:                                                ; preds = %1
  %10 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m) #4
  %11 = load %struct.DEVICE*, %struct.DEVICE** %3, align 8
  %12 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %11, i32 0, i32 0
  %13 = load i32, i32* %12, align 4
  %14 = add nsw i32 %13, 1
  %15 = load %struct.DEVICE*, %struct.DEVICE** %3, align 8
  %16 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %15, i32 0, i32 0
  store i32 %14, i32* %16, align 4
  %17 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m) #4
  store i32 0, i32* %2, align 4
  br label %18

18:                                               ; preds = %9, %8
  %19 = load i32, i32* %2, align 4
  ret i32 %19
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_lock(%union.pthread_mutex_t*) #1

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_unlock(%union.pthread_mutex_t*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @BCSP_IoDecrement(%struct.DEVICE* %0) #0 {
  %2 = alloca %struct.DEVICE*, align 8
  %3 = alloca i32, align 4
  store %struct.DEVICE* %0, %struct.DEVICE** %2, align 8
  %4 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* @m) #4
  %5 = load %struct.DEVICE*, %struct.DEVICE** %2, align 8
  %6 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %5, i32 0, i32 0
  %7 = load i32, i32* %6, align 4
  %8 = sub nsw i32 %7, 1
  %9 = load %struct.DEVICE*, %struct.DEVICE** %2, align 8
  %10 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %9, i32 0, i32 0
  store i32 %8, i32* %10, align 4
  %11 = load %struct.DEVICE*, %struct.DEVICE** %2, align 8
  %12 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %11, i32 0, i32 0
  %13 = load i32, i32* %12, align 4
  store i32 %13, i32* %3, align 4
  %14 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* @m) #4
  %15 = load i32, i32* %3, align 4
  %16 = icmp eq i32 %15, 0
  br i1 %16, label %17, label %20

17:                                               ; preds = %1
  %18 = load %struct.DEVICE*, %struct.DEVICE** %2, align 8
  %19 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %18, i32 0, i32 2
  store i8 1, i8* %19, align 1
  br label %20

20:                                               ; preds = %17, %1
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @BCSP_PnpAdd(%struct.DEVICE* %0) #0 {
  %2 = alloca %struct.DEVICE*, align 8
  %3 = alloca i32, align 4
  store %struct.DEVICE* %0, %struct.DEVICE** %2, align 8
  %4 = load %struct.DEVICE*, %struct.DEVICE** %2, align 8
  %5 = call i32 @BCSP_IoIncrement(%struct.DEVICE* %4)
  store i32 %5, i32* %3, align 4
  %6 = load i32, i32* %3, align 4
  %7 = icmp eq i32 %6, 0
  br i1 %7, label %8, label %14

8:                                                ; preds = %1
  %9 = load i8, i8* @stopped, align 1
  %10 = trunc i8 %9 to i1
  br i1 %10, label %12, label %11

11:                                               ; preds = %8
  br label %13

12:                                               ; preds = %8
  call void @__assert_fail(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str, i64 0, i64 0), i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.1, i64 0, i64 0), i32 57, i8* getelementptr inbounds ([37 x i8], [37 x i8]* @__PRETTY_FUNCTION__.BCSP_PnpAdd, i64 0, i64 0)) #5
  unreachable

13:                                               ; preds = %11
  br label %14

14:                                               ; preds = %13, %1
  %15 = load %struct.DEVICE*, %struct.DEVICE** %2, align 8
  call void @BCSP_IoDecrement(%struct.DEVICE* %15)
  ret void
}

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i8* @BCSP_PnpStop(i8* %0) #0 {
  %2 = alloca i8*, align 8
  %3 = alloca i8*, align 8
  %4 = alloca %struct.DEVICE*, align 8
  store i8* %0, i8** %3, align 8
  %5 = load i8*, i8** %3, align 8
  %6 = bitcast i8* %5 to %struct.DEVICE*
  store %struct.DEVICE* %6, %struct.DEVICE** %4, align 8
  %7 = load %struct.DEVICE*, %struct.DEVICE** %4, align 8
  %8 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %7, i32 0, i32 1
  store i8 1, i8* %8, align 4
  %9 = load %struct.DEVICE*, %struct.DEVICE** %4, align 8
  call void @BCSP_IoDecrement(%struct.DEVICE* %9)
  %10 = load %struct.DEVICE*, %struct.DEVICE** %4, align 8
  %11 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %10, i32 0, i32 2
  %12 = load i8, i8* %11, align 1
  %13 = trunc i8 %12 to i1
  br i1 %13, label %14, label %15

14:                                               ; preds = %1
  store i8 1, i8* @stopped, align 1
  br label %15

15:                                               ; preds = %14, %1
  %16 = load i8*, i8** %2, align 8
  ret i8* %16
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @main() #0 {
  %1 = alloca i64, align 8
  %2 = alloca %struct.DEVICE, align 4
  %3 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* @m, %union.pthread_mutexattr_t* null) #4
  %4 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %2, i32 0, i32 0
  store i32 1, i32* %4, align 4
  %5 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %2, i32 0, i32 1
  store i8 0, i8* %5, align 4
  %6 = getelementptr inbounds %struct.DEVICE, %struct.DEVICE* %2, i32 0, i32 2
  store i8 0, i8* %6, align 1
  store i8 0, i8* @stopped, align 1
  %7 = bitcast %struct.DEVICE* %2 to i8*
  %8 = call i32 @pthread_create(i64* %1, %union.pthread_attr_t* null, i8* (i8*)* @BCSP_PnpStop, i8* %7) #4
  call void @BCSP_PnpAdd(%struct.DEVICE* %2)
  %9 = load i64, i64* %1, align 8
  %10 = call i32 @pthread_join(i64 %9, i8** null)
  ret void
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_init(%union.pthread_mutex_t*, %union.pthread_mutexattr_t*) #1

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #1

declare dso_local i32 @pthread_join(i64, i8**) #3

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }
attributes #5 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1 (https://github.com/llvm/llvm-project.git 43ff75f2c3feef64f9d73328230d34dac8832a91)"}
