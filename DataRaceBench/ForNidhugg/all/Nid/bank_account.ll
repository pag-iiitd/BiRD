; ModuleID = 'bank_account.cc'
source_filename = "bank_account.cc"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.bank_account_type = type { i32, %union.pthread_mutex_t }
%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_mutexattr_t = type { i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

@.str = private unnamed_addr constant [21 x i8] c"t1 is depositing %d\0A\00", align 1
@_ZL6amount = internal global i32 20, align 4
@.str.1 = private unnamed_addr constant [14 x i8] c"deposit done\0A\00", align 1
@.str.2 = private unnamed_addr constant [22 x i8] c"t1 is withdrawing %d\0A\00", align 1
@.str.3 = private unnamed_addr constant [15 x i8] c"withdraw done\0A\00", align 1
@.str.4 = private unnamed_addr constant [14 x i8] c"balance = %d\0A\00", align 1
@.str.5 = private unnamed_addr constant [22 x i8] c"account->balance == 0\00", align 1
@.str.6 = private unnamed_addr constant [16 x i8] c"bank_account.cc\00", align 1
@__PRETTY_FUNCTION__.main = private unnamed_addr constant [23 x i8] c"int main(int, char **)\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @_Z11get_balanceP17bank_account_type(%struct.bank_account_type* %0) #0 {
  %2 = alloca %struct.bank_account_type*, align 8
  %3 = alloca i32, align 4
  store %struct.bank_account_type* %0, %struct.bank_account_type** %2, align 8
  store i32 0, i32* %3, align 4
  %4 = load %struct.bank_account_type*, %struct.bank_account_type** %2, align 8
  %5 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %4, i32 0, i32 1
  %6 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* %5) #7
  %7 = load %struct.bank_account_type*, %struct.bank_account_type** %2, align 8
  %8 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %7, i32 0, i32 0
  %9 = load i32, i32* %8, align 8
  store i32 %9, i32* %3, align 4
  %10 = load %struct.bank_account_type*, %struct.bank_account_type** %2, align 8
  %11 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %10, i32 0, i32 1
  %12 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* %11) #7
  %13 = load i32, i32* %3, align 4
  ret i32 %13
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_lock(%union.pthread_mutex_t*) #1

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_unlock(%union.pthread_mutex_t*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @_Z11set_balanceP17bank_account_typei(%struct.bank_account_type* %0, i32 %1) #0 {
  %3 = alloca %struct.bank_account_type*, align 8
  %4 = alloca i32, align 4
  store %struct.bank_account_type* %0, %struct.bank_account_type** %3, align 8
  store i32 %1, i32* %4, align 4
  %5 = load %struct.bank_account_type*, %struct.bank_account_type** %3, align 8
  %6 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %5, i32 0, i32 1
  %7 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* %6) #7
  %8 = load i32, i32* %4, align 4
  %9 = load %struct.bank_account_type*, %struct.bank_account_type** %3, align 8
  %10 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %9, i32 0, i32 0
  store i32 %8, i32* %10, align 8
  %11 = load %struct.bank_account_type*, %struct.bank_account_type** %3, align 8
  %12 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %11, i32 0, i32 1
  %13 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* %12) #7
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @_Z8withdrawP17bank_account_typei(%struct.bank_account_type* %0, i32 %1) #0 {
  %3 = alloca %struct.bank_account_type*, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store %struct.bank_account_type* %0, %struct.bank_account_type** %3, align 8
  store i32 %1, i32* %4, align 4
  store i32 0, i32* %5, align 4
  %6 = load %struct.bank_account_type*, %struct.bank_account_type** %3, align 8
  %7 = call i32 @_Z11get_balanceP17bank_account_type(%struct.bank_account_type* %6)
  store i32 %7, i32* %5, align 4
  %8 = load i32, i32* %4, align 4
  %9 = load i32, i32* %5, align 4
  %10 = sub nsw i32 %9, %8
  store i32 %10, i32* %5, align 4
  %11 = load %struct.bank_account_type*, %struct.bank_account_type** %3, align 8
  %12 = load i32, i32* %5, align 4
  call void @_Z11set_balanceP17bank_account_typei(%struct.bank_account_type* %11, i32 %12)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @_Z7depositP17bank_account_typei(%struct.bank_account_type* %0, i32 %1) #0 {
  %3 = alloca %struct.bank_account_type*, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store %struct.bank_account_type* %0, %struct.bank_account_type** %3, align 8
  store i32 %1, i32* %4, align 4
  store i32 0, i32* %5, align 4
  %6 = load %struct.bank_account_type*, %struct.bank_account_type** %3, align 8
  %7 = call i32 @_Z11get_balanceP17bank_account_type(%struct.bank_account_type* %6)
  store i32 %7, i32* %5, align 4
  %8 = load i32, i32* %4, align 4
  %9 = load i32, i32* %5, align 4
  %10 = add nsw i32 %9, %8
  store i32 %10, i32* %5, align 4
  %11 = load %struct.bank_account_type*, %struct.bank_account_type** %3, align 8
  %12 = load i32, i32* %5, align 4
  call void @_Z11set_balanceP17bank_account_typei(%struct.bank_account_type* %11, i32 %12)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @_Z12init_accountP17bank_account_type(%struct.bank_account_type* %0) #0 {
  %2 = alloca %struct.bank_account_type*, align 8
  store %struct.bank_account_type* %0, %struct.bank_account_type** %2, align 8
  %3 = load %struct.bank_account_type*, %struct.bank_account_type** %2, align 8
  %4 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %3, i32 0, i32 0
  store i32 0, i32* %4, align 8
  %5 = load %struct.bank_account_type*, %struct.bank_account_type** %2, align 8
  %6 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %5, i32 0, i32 1
  %7 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* %6, %union.pthread_mutexattr_t* null) #7
  ret void
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_init(%union.pthread_mutex_t*, %union.pthread_mutexattr_t*) #1

; Function Attrs: noinline optnone uwtable
define dso_local i8* @_Z7t1_mainPv(i8* %0) #2 {
  %2 = alloca i8*, align 8
  %3 = alloca %struct.bank_account_type*, align 8
  store i8* %0, i8** %2, align 8
  %4 = load i8*, i8** %2, align 8
  %5 = bitcast i8* %4 to %struct.bank_account_type*
  store %struct.bank_account_type* %5, %struct.bank_account_type** %3, align 8
  %6 = load i32, i32* @_ZL6amount, align 4
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str, i64 0, i64 0), i32 %6)
  %8 = load %struct.bank_account_type*, %struct.bank_account_type** %3, align 8
  %9 = load i32, i32* @_ZL6amount, align 4
  call void @_Z7depositP17bank_account_typei(%struct.bank_account_type* %8, i32 %9)
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.1, i64 0, i64 0))
  ret i8* null
}

declare dso_local i32 @printf(i8*, ...) #3

; Function Attrs: noinline optnone uwtable
define dso_local i8* @_Z7t2_mainPv(i8* %0) #2 {
  %2 = alloca i8*, align 8
  %3 = alloca %struct.bank_account_type*, align 8
  store i8* %0, i8** %2, align 8
  %4 = load i8*, i8** %2, align 8
  %5 = bitcast i8* %4 to %struct.bank_account_type*
  store %struct.bank_account_type* %5, %struct.bank_account_type** %3, align 8
  %6 = load i32, i32* @_ZL6amount, align 4
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.2, i64 0, i64 0), i32 %6)
  %8 = load %struct.bank_account_type*, %struct.bank_account_type** %3, align 8
  call void @_Z8withdrawP17bank_account_typei(%struct.bank_account_type* %8, i32 20)
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str.3, i64 0, i64 0))
  ret i8* null
}

; Function Attrs: noinline norecurse optnone uwtable
define dso_local i32 @main(i32 %0, i8** %1) #4 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca %struct.bank_account_type*, align 8
  %7 = alloca i64*, align 8
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  %8 = call noalias nonnull i8* @_Znwm(i64 48) #8
  %9 = bitcast i8* %8 to %struct.bank_account_type*
  store %struct.bank_account_type* %9, %struct.bank_account_type** %6, align 8
  %10 = load %struct.bank_account_type*, %struct.bank_account_type** %6, align 8
  call void @_Z12init_accountP17bank_account_type(%struct.bank_account_type* %10)
  %11 = call noalias nonnull i8* @_Znam(i64 16) #8
  %12 = bitcast i8* %11 to i64*
  store i64* %12, i64** %7, align 8
  %13 = load i64*, i64** %7, align 8
  %14 = getelementptr inbounds i64, i64* %13, i64 0
  %15 = load %struct.bank_account_type*, %struct.bank_account_type** %6, align 8
  %16 = bitcast %struct.bank_account_type* %15 to i8*
  %17 = call i32 @pthread_create(i64* %14, %union.pthread_attr_t* null, i8* (i8*)* @_Z7t1_mainPv, i8* %16) #7
  %18 = load i64*, i64** %7, align 8
  %19 = getelementptr inbounds i64, i64* %18, i64 1
  %20 = load %struct.bank_account_type*, %struct.bank_account_type** %6, align 8
  %21 = bitcast %struct.bank_account_type* %20 to i8*
  %22 = call i32 @pthread_create(i64* %19, %union.pthread_attr_t* null, i8* (i8*)* @_Z7t2_mainPv, i8* %21) #7
  %23 = load i64*, i64** %7, align 8
  %24 = getelementptr inbounds i64, i64* %23, i64 0
  %25 = load i64, i64* %24, align 8
  %26 = call i32 @pthread_join(i64 %25, i8** null)
  %27 = load i64*, i64** %7, align 8
  %28 = getelementptr inbounds i64, i64* %27, i64 1
  %29 = load i64, i64* %28, align 8
  %30 = call i32 @pthread_join(i64 %29, i8** null)
  %31 = load %struct.bank_account_type*, %struct.bank_account_type** %6, align 8
  %32 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %31, i32 0, i32 0
  %33 = load i32, i32* %32, align 8
  %34 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.4, i64 0, i64 0), i32 %33)
  %35 = load %struct.bank_account_type*, %struct.bank_account_type** %6, align 8
  %36 = getelementptr inbounds %struct.bank_account_type, %struct.bank_account_type* %35, i32 0, i32 0
  %37 = load i32, i32* %36, align 8
  %38 = icmp eq i32 %37, 0
  br i1 %38, label %39, label %40

39:                                               ; preds = %2
  br label %42

40:                                               ; preds = %2
  call void @__assert_fail(i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.5, i64 0, i64 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.6, i64 0, i64 0), i32 92, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @__PRETTY_FUNCTION__.main, i64 0, i64 0)) #9
  unreachable

41:                                               ; No predecessors!
  br label %42

42:                                               ; preds = %41, %39
  ret i32 0
}

; Function Attrs: nobuiltin allocsize(0)
declare dso_local nonnull i8* @_Znwm(i64) #5

; Function Attrs: nobuiltin allocsize(0)
declare dso_local nonnull i8* @_Znam(i64) #5

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #1

declare dso_local i32 @pthread_join(i64, i8**) #3

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #6

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noinline optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nobuiltin allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { nounwind }
attributes #8 = { builtin allocsize(0) }
attributes #9 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1 (https://github.com/llvm/llvm-project.git 43ff75f2c3feef64f9d73328230d34dac8832a91)"}
