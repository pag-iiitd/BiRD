; ModuleID = 'stringbuffer.cc'
source_filename = "stringbuffer.cc"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%class.StringBuffer = type { i8*, i32, i32, %union.pthread_mutex_t }
%union.pthread_mutex_t = type { %struct.__pthread_mutex_s }
%struct.__pthread_mutex_s = type { i32, i32, i32, i32, i32, i16, i16, %struct.__pthread_internal_list }
%struct.__pthread_internal_list = type { %struct.__pthread_internal_list*, %struct.__pthread_internal_list* }
%union.pthread_mutexattr_t = type { i32 }
%union.pthread_attr_t = type { i64, [48 x i8] }

$_ZN12StringBufferC2EPKc = comdat any

$_ZN12StringBuffer5eraseEii = comdat any

$_ZN12StringBufferC2Ev = comdat any

$_ZN12StringBuffer6appendEPS_ = comdat any

$_ZN12StringBuffer6appendEPKc = comdat any

$_ZN12StringBuffer14expandCapacityEi = comdat any

$_ZN12StringBuffer6lengthEv = comdat any

$_ZN12StringBuffer8getCharsEiiPci = comdat any

@_ZN12StringBuffer11null_bufferE = dso_local global %class.StringBuffer* null, align 8
@.str = private unnamed_addr constant [5 x i8] c"null\00", align 1
@buffer = dso_local global %class.StringBuffer* null, align 8
@.str.2 = private unnamed_addr constant [4 x i8] c"abc\00", align 1
@.str.3 = private unnamed_addr constant [20 x i8] c"erasing the buffer\0A\00", align 1
@.str.4 = private unnamed_addr constant [14 x i8] c"erasing done\0A\00", align 1
@.str.5 = private unnamed_addr constant [22 x i8] c"appending the buffer\0A\00", align 1
@.str.6 = private unnamed_addr constant [16 x i8] c"appending done\0A\00", align 1
@.str.7 = private unnamed_addr constant [2 x i8] c"0\00", align 1
@.str.8 = private unnamed_addr constant [16 x i8] c"stringbuffer.cc\00", align 1
@__PRETTY_FUNCTION__._ZN12StringBuffer5eraseEii = private unnamed_addr constant [44 x i8] c"StringBuffer *StringBuffer::erase(int, int)\00", align 1
@__PRETTY_FUNCTION__._ZN12StringBuffer8getCharsEiiPci = private unnamed_addr constant [51 x i8] c"void StringBuffer::getChars(int, int, char *, int)\00", align 1
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_stringbuffer.cc, i8* null }]

; Function Attrs: noinline uwtable
define internal void @__cxx_global_var_init() #0 section ".text.startup" personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
  %1 = alloca i8*, align 8
  %2 = alloca i32, align 4
  %3 = call noalias nonnull i8* @_Znwm(i64 56) #11
  %4 = bitcast i8* %3 to %class.StringBuffer*
  invoke void @_ZN12StringBufferC2EPKc(%class.StringBuffer* %4, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i64 0, i64 0))
          to label %5 unwind label %6

5:                                                ; preds = %0
  store %class.StringBuffer* %4, %class.StringBuffer** @_ZN12StringBuffer11null_bufferE, align 8
  ret void

6:                                                ; preds = %0
  %7 = landingpad { i8*, i32 }
          cleanup
  %8 = extractvalue { i8*, i32 } %7, 0
  store i8* %8, i8** %1, align 8
  %9 = extractvalue { i8*, i32 } %7, 1
  store i32 %9, i32* %2, align 4
  call void @_ZdlPv(i8* %3) #12
  br label %10

10:                                               ; preds = %6
  %11 = load i8*, i8** %1, align 8
  %12 = load i32, i32* %2, align 4
  %13 = insertvalue { i8*, i32 } undef, i8* %11, 0
  %14 = insertvalue { i8*, i32 } %13, i32 %12, 1
  resume { i8*, i32 } %14
}

; Function Attrs: nobuiltin allocsize(0)
declare dso_local nonnull i8* @_Znwm(i64) #1

; Function Attrs: noinline optnone uwtable
define linkonce_odr dso_local void @_ZN12StringBufferC2EPKc(%class.StringBuffer* %0, i8* %1) unnamed_addr #2 comdat align 2 {
  %3 = alloca %class.StringBuffer*, align 8
  %4 = alloca i8*, align 8
  %5 = alloca i32, align 4
  store %class.StringBuffer* %0, %class.StringBuffer** %3, align 8
  store i8* %1, i8** %4, align 8
  %6 = load %class.StringBuffer*, %class.StringBuffer** %3, align 8
  %7 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %6, i32 0, i32 3
  %8 = load i8*, i8** %4, align 8
  %9 = call i64 @strlen(i8* %8) #13
  %10 = add i64 %9, 16
  %11 = trunc i64 %10 to i32
  store i32 %11, i32* %5, align 4
  %12 = load i32, i32* %5, align 4
  %13 = sext i32 %12 to i64
  %14 = call noalias nonnull i8* @_Znam(i64 %13) #11
  %15 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %6, i32 0, i32 0
  store i8* %14, i8** %15, align 8
  %16 = load i32, i32* %5, align 4
  %17 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %6, i32 0, i32 1
  store i32 %16, i32* %17, align 8
  %18 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %6, i32 0, i32 2
  store i32 0, i32* %18, align 4
  %19 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %6, i32 0, i32 3
  %20 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* %19, %union.pthread_mutexattr_t* null) #14
  %21 = load i8*, i8** %4, align 8
  %22 = call %class.StringBuffer* @_ZN12StringBuffer6appendEPKc(%class.StringBuffer* %6, i8* %21)
  ret void
}

declare dso_local i32 @__gxx_personality_v0(...)

; Function Attrs: nobuiltin nounwind
declare dso_local void @_ZdlPv(i8*) #3

; Function Attrs: noinline uwtable
define internal void @__cxx_global_var_init.1() #0 section ".text.startup" personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
  %1 = alloca i8*, align 8
  %2 = alloca i32, align 4
  %3 = call noalias nonnull i8* @_Znwm(i64 56) #11
  %4 = bitcast i8* %3 to %class.StringBuffer*
  invoke void @_ZN12StringBufferC2EPKc(%class.StringBuffer* %4, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.2, i64 0, i64 0))
          to label %5 unwind label %6

5:                                                ; preds = %0
  store %class.StringBuffer* %4, %class.StringBuffer** @buffer, align 8
  ret void

6:                                                ; preds = %0
  %7 = landingpad { i8*, i32 }
          cleanup
  %8 = extractvalue { i8*, i32 } %7, 0
  store i8* %8, i8** %1, align 8
  %9 = extractvalue { i8*, i32 } %7, 1
  store i32 %9, i32* %2, align 4
  call void @_ZdlPv(i8* %3) #12
  br label %10

10:                                               ; preds = %6
  %11 = load i8*, i8** %1, align 8
  %12 = load i32, i32* %2, align 4
  %13 = insertvalue { i8*, i32 } undef, i8* %11, 0
  %14 = insertvalue { i8*, i32 } %13, i32 %12, 1
  resume { i8*, i32 } %14
}

; Function Attrs: noinline optnone uwtable
define dso_local i8* @_Z11thread_mainPv(i8* %0) #2 {
  %2 = alloca i8*, align 8
  store i8* %0, i8** %2, align 8
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @.str.3, i64 0, i64 0))
  %4 = load %class.StringBuffer*, %class.StringBuffer** @buffer, align 8
  %5 = call %class.StringBuffer* @_ZN12StringBuffer5eraseEii(%class.StringBuffer* %4, i32 0, i32 3)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str.4, i64 0, i64 0))
  ret i8* null
}

declare dso_local i32 @printf(i8*, ...) #4

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local %class.StringBuffer* @_ZN12StringBuffer5eraseEii(%class.StringBuffer* %0, i32 %1, i32 %2) #5 comdat align 2 {
  %4 = alloca %class.StringBuffer*, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store %class.StringBuffer* %0, %class.StringBuffer** %4, align 8
  store i32 %1, i32* %5, align 4
  store i32 %2, i32* %6, align 4
  %8 = load %class.StringBuffer*, %class.StringBuffer** %4, align 8
  %9 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %8, i32 0, i32 3
  %10 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* %9) #14
  %11 = load i32, i32* %5, align 4
  %12 = icmp slt i32 %11, 0
  br i1 %12, label %13, label %14

13:                                               ; preds = %3
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.7, i64 0, i64 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.8, i64 0, i64 0), i32 110, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @__PRETTY_FUNCTION__._ZN12StringBuffer5eraseEii, i64 0, i64 0)) #15
  unreachable

14:                                               ; preds = %3
  %15 = load i32, i32* %6, align 4
  %16 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %8, i32 0, i32 2
  %17 = load i32, i32* %16, align 4
  %18 = icmp sgt i32 %15, %17
  br i1 %18, label %19, label %22

19:                                               ; preds = %14
  %20 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %8, i32 0, i32 2
  %21 = load i32, i32* %20, align 4
  store i32 %21, i32* %6, align 4
  br label %22

22:                                               ; preds = %19, %14
  %23 = load i32, i32* %5, align 4
  %24 = load i32, i32* %6, align 4
  %25 = icmp sgt i32 %23, %24
  br i1 %25, label %26, label %27

26:                                               ; preds = %22
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.7, i64 0, i64 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.8, i64 0, i64 0), i32 114, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @__PRETTY_FUNCTION__._ZN12StringBuffer5eraseEii, i64 0, i64 0)) #15
  unreachable

27:                                               ; preds = %22
  %28 = load i32, i32* %6, align 4
  %29 = load i32, i32* %5, align 4
  %30 = sub nsw i32 %28, %29
  store i32 %30, i32* %7, align 4
  %31 = load i32, i32* %7, align 4
  %32 = icmp sgt i32 %31, 0
  br i1 %32, label %33, label %56

33:                                               ; preds = %27
  %34 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %8, i32 0, i32 0
  %35 = load i8*, i8** %34, align 8
  %36 = load i32, i32* %5, align 4
  %37 = sext i32 %36 to i64
  %38 = getelementptr inbounds i8, i8* %35, i64 %37
  %39 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %8, i32 0, i32 0
  %40 = load i8*, i8** %39, align 8
  %41 = load i32, i32* %5, align 4
  %42 = sext i32 %41 to i64
  %43 = getelementptr inbounds i8, i8* %40, i64 %42
  %44 = load i32, i32* %7, align 4
  %45 = sext i32 %44 to i64
  %46 = getelementptr inbounds i8, i8* %43, i64 %45
  %47 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %8, i32 0, i32 2
  %48 = load i32, i32* %47, align 4
  %49 = load i32, i32* %6, align 4
  %50 = sub nsw i32 %48, %49
  %51 = sext i32 %50 to i64
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %38, i8* align 1 %46, i64 %51, i1 false)
  %52 = load i32, i32* %7, align 4
  %53 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %8, i32 0, i32 2
  %54 = load i32, i32* %53, align 4
  %55 = sub nsw i32 %54, %52
  store i32 %55, i32* %53, align 4
  br label %56

56:                                               ; preds = %33, %27
  %57 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %8, i32 0, i32 3
  %58 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* %57) #14
  ret %class.StringBuffer* %8
}

; Function Attrs: noinline norecurse optnone uwtable
define dso_local i32 @main(i32 %0, i8** %1) #6 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i64, align 8
  %7 = alloca %class.StringBuffer*, align 8
  %8 = alloca i8*, align 8
  %9 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  %10 = call i32 @pthread_create(i64* %6, %union.pthread_attr_t* null, i8* (i8*)* @_Z11thread_mainPv, i8* null) #14
  %11 = call noalias nonnull i8* @_Znwm(i64 56) #11
  %12 = bitcast i8* %11 to %class.StringBuffer*
  invoke void @_ZN12StringBufferC2Ev(%class.StringBuffer* %12)
          to label %13 unwind label %21

13:                                               ; preds = %2
  store %class.StringBuffer* %12, %class.StringBuffer** %7, align 8
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.5, i64 0, i64 0))
  %15 = load %class.StringBuffer*, %class.StringBuffer** %7, align 8
  %16 = load %class.StringBuffer*, %class.StringBuffer** @buffer, align 8
  %17 = call %class.StringBuffer* @_ZN12StringBuffer6appendEPS_(%class.StringBuffer* %15, %class.StringBuffer* %16)
  %18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.6, i64 0, i64 0))
  %19 = load i64, i64* %6, align 8
  %20 = call i32 @pthread_join(i64 %19, i8** null)
  ret i32 0

21:                                               ; preds = %2
  %22 = landingpad { i8*, i32 }
          cleanup
  %23 = extractvalue { i8*, i32 } %22, 0
  store i8* %23, i8** %8, align 8
  %24 = extractvalue { i8*, i32 } %22, 1
  store i32 %24, i32* %9, align 4
  call void @_ZdlPv(i8* %11) #12
  br label %25

25:                                               ; preds = %21
  %26 = load i8*, i8** %8, align 8
  %27 = load i32, i32* %9, align 4
  %28 = insertvalue { i8*, i32 } undef, i8* %26, 0
  %29 = insertvalue { i8*, i32 } %28, i32 %27, 1
  resume { i8*, i32 } %29
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_create(i64*, %union.pthread_attr_t*, i8* (i8*)*, i8*) #7

; Function Attrs: noinline optnone uwtable
define linkonce_odr dso_local void @_ZN12StringBufferC2Ev(%class.StringBuffer* %0) unnamed_addr #2 comdat align 2 {
  %2 = alloca %class.StringBuffer*, align 8
  store %class.StringBuffer* %0, %class.StringBuffer** %2, align 8
  %3 = load %class.StringBuffer*, %class.StringBuffer** %2, align 8
  %4 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %3, i32 0, i32 3
  %5 = call noalias nonnull i8* @_Znam(i64 16) #11
  %6 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %3, i32 0, i32 0
  store i8* %5, i8** %6, align 8
  %7 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %3, i32 0, i32 1
  store i32 16, i32* %7, align 8
  %8 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %3, i32 0, i32 2
  store i32 0, i32* %8, align 4
  %9 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %3, i32 0, i32 3
  %10 = call i32 @pthread_mutex_init(%union.pthread_mutex_t* %9, %union.pthread_mutexattr_t* null) #14
  ret void
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr dso_local %class.StringBuffer* @_ZN12StringBuffer6appendEPS_(%class.StringBuffer* %0, %class.StringBuffer* %1) #2 comdat align 2 {
  %3 = alloca %class.StringBuffer*, align 8
  %4 = alloca %class.StringBuffer*, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store %class.StringBuffer* %0, %class.StringBuffer** %3, align 8
  store %class.StringBuffer* %1, %class.StringBuffer** %4, align 8
  %7 = load %class.StringBuffer*, %class.StringBuffer** %3, align 8
  %8 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 3
  %9 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* %8) #14
  %10 = load %class.StringBuffer*, %class.StringBuffer** %4, align 8
  %11 = icmp eq %class.StringBuffer* %10, null
  br i1 %11, label %12, label %14

12:                                               ; preds = %2
  %13 = load %class.StringBuffer*, %class.StringBuffer** @_ZN12StringBuffer11null_bufferE, align 8
  store %class.StringBuffer* %13, %class.StringBuffer** %4, align 8
  br label %14

14:                                               ; preds = %12, %2
  %15 = load %class.StringBuffer*, %class.StringBuffer** %4, align 8
  %16 = call i32 @_ZN12StringBuffer6lengthEv(%class.StringBuffer* %15)
  store i32 %16, i32* %5, align 4
  %17 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 2
  %18 = load i32, i32* %17, align 4
  %19 = load i32, i32* %5, align 4
  %20 = add nsw i32 %18, %19
  store i32 %20, i32* %6, align 4
  %21 = load i32, i32* %6, align 4
  %22 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 1
  %23 = load i32, i32* %22, align 8
  %24 = icmp sgt i32 %21, %23
  br i1 %24, label %25, label %27

25:                                               ; preds = %14
  %26 = load i32, i32* %6, align 4
  call void @_ZN12StringBuffer14expandCapacityEi(%class.StringBuffer* %7, i32 %26)
  br label %27

27:                                               ; preds = %25, %14
  %28 = load %class.StringBuffer*, %class.StringBuffer** %4, align 8
  %29 = load i32, i32* %5, align 4
  %30 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 0
  %31 = load i8*, i8** %30, align 8
  %32 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 2
  %33 = load i32, i32* %32, align 4
  call void @_ZN12StringBuffer8getCharsEiiPci(%class.StringBuffer* %28, i32 0, i32 %29, i8* %31, i32 %33)
  %34 = load i32, i32* %6, align 4
  %35 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 2
  store i32 %34, i32* %35, align 4
  %36 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 3
  %37 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* %36) #14
  ret %class.StringBuffer* %7
}

declare dso_local i32 @pthread_join(i64, i8**) #4

; Function Attrs: nounwind readonly
declare dso_local i64 @strlen(i8*) #8

; Function Attrs: nobuiltin allocsize(0)
declare dso_local nonnull i8* @_Znam(i64) #1

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_init(%union.pthread_mutex_t*, %union.pthread_mutexattr_t*) #7

; Function Attrs: noinline optnone uwtable
define linkonce_odr dso_local %class.StringBuffer* @_ZN12StringBuffer6appendEPKc(%class.StringBuffer* %0, i8* %1) #2 comdat align 2 {
  %3 = alloca %class.StringBuffer*, align 8
  %4 = alloca i8*, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store %class.StringBuffer* %0, %class.StringBuffer** %3, align 8
  store i8* %1, i8** %4, align 8
  %7 = load %class.StringBuffer*, %class.StringBuffer** %3, align 8
  %8 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 3
  %9 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* %8) #14
  %10 = load i8*, i8** %4, align 8
  %11 = icmp eq i8* %10, null
  br i1 %11, label %12, label %13

12:                                               ; preds = %2
  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i64 0, i64 0), i8** %4, align 8
  br label %13

13:                                               ; preds = %12, %2
  %14 = load i8*, i8** %4, align 8
  %15 = call i64 @strlen(i8* %14) #13
  %16 = trunc i64 %15 to i32
  store i32 %16, i32* %5, align 4
  %17 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 2
  %18 = load i32, i32* %17, align 4
  %19 = load i32, i32* %5, align 4
  %20 = add nsw i32 %18, %19
  store i32 %20, i32* %6, align 4
  %21 = load i32, i32* %6, align 4
  %22 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 1
  %23 = load i32, i32* %22, align 8
  %24 = icmp sgt i32 %21, %23
  br i1 %24, label %25, label %27

25:                                               ; preds = %13
  %26 = load i32, i32* %6, align 4
  call void @_ZN12StringBuffer14expandCapacityEi(%class.StringBuffer* %7, i32 %26)
  br label %27

27:                                               ; preds = %25, %13
  %28 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 0
  %29 = load i8*, i8** %28, align 8
  %30 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 2
  %31 = load i32, i32* %30, align 4
  %32 = sext i32 %31 to i64
  %33 = getelementptr inbounds i8, i8* %29, i64 %32
  %34 = load i8*, i8** %4, align 8
  %35 = load i32, i32* %5, align 4
  %36 = sext i32 %35 to i64
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %33, i8* align 1 %34, i64 %36, i1 false)
  %37 = load i32, i32* %6, align 4
  %38 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 2
  store i32 %37, i32* %38, align 4
  %39 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 3
  %40 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* %39) #14
  ret %class.StringBuffer* %7
}

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_lock(%union.pthread_mutex_t*) #7

; Function Attrs: noinline optnone uwtable
define linkonce_odr dso_local void @_ZN12StringBuffer14expandCapacityEi(%class.StringBuffer* %0, i32 %1) #2 comdat align 2 {
  %3 = alloca %class.StringBuffer*, align 8
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i8*, align 8
  store %class.StringBuffer* %0, %class.StringBuffer** %3, align 8
  store i32 %1, i32* %4, align 4
  %7 = load %class.StringBuffer*, %class.StringBuffer** %3, align 8
  %8 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 1
  %9 = load i32, i32* %8, align 8
  %10 = add nsw i32 %9, 1
  %11 = mul nsw i32 %10, 2
  store i32 %11, i32* %5, align 4
  %12 = load i32, i32* %5, align 4
  %13 = icmp slt i32 %12, 0
  br i1 %13, label %14, label %15

14:                                               ; preds = %2
  store i32 2147483647, i32* %5, align 4
  br label %22

15:                                               ; preds = %2
  %16 = load i32, i32* %4, align 4
  %17 = load i32, i32* %5, align 4
  %18 = icmp sgt i32 %16, %17
  br i1 %18, label %19, label %21

19:                                               ; preds = %15
  %20 = load i32, i32* %4, align 4
  store i32 %20, i32* %5, align 4
  br label %21

21:                                               ; preds = %19, %15
  br label %22

22:                                               ; preds = %21, %14
  %23 = load i32, i32* %5, align 4
  %24 = sext i32 %23 to i64
  %25 = call noalias nonnull i8* @_Znam(i64 %24) #11
  store i8* %25, i8** %6, align 8
  %26 = load i8*, i8** %6, align 8
  %27 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 0
  %28 = load i8*, i8** %27, align 8
  %29 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 2
  %30 = load i32, i32* %29, align 4
  %31 = sext i32 %30 to i64
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %26, i8* align 1 %28, i64 %31, i1 false)
  %32 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 0
  %33 = load i8*, i8** %32, align 8
  %34 = icmp eq i8* %33, null
  br i1 %34, label %36, label %35

35:                                               ; preds = %22
  call void @_ZdaPv(i8* %33) #12
  br label %36

36:                                               ; preds = %35, %22
  %37 = load i8*, i8** %6, align 8
  %38 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 0
  store i8* %37, i8** %38, align 8
  %39 = load i32, i32* %5, align 4
  %40 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %7, i32 0, i32 1
  store i32 %39, i32* %40, align 8
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #9

; Function Attrs: nounwind
declare dso_local i32 @pthread_mutex_unlock(%union.pthread_mutex_t*) #7

; Function Attrs: nobuiltin nounwind
declare dso_local void @_ZdaPv(i8*) #3

; Function Attrs: noreturn nounwind
declare dso_local void @__assert_fail(i8*, i8*, i32, i8*) #10

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local i32 @_ZN12StringBuffer6lengthEv(%class.StringBuffer* %0) #5 comdat align 2 {
  %2 = alloca %class.StringBuffer*, align 8
  %3 = alloca i32, align 4
  store %class.StringBuffer* %0, %class.StringBuffer** %2, align 8
  %4 = load %class.StringBuffer*, %class.StringBuffer** %2, align 8
  %5 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %4, i32 0, i32 3
  %6 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* %5) #14
  %7 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %4, i32 0, i32 2
  %8 = load i32, i32* %7, align 4
  store i32 %8, i32* %3, align 4
  %9 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %4, i32 0, i32 3
  %10 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* %9) #14
  %11 = load i32, i32* %3, align 4
  ret i32 %11
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dso_local void @_ZN12StringBuffer8getCharsEiiPci(%class.StringBuffer* %0, i32 %1, i32 %2, i8* %3, i32 %4) #5 comdat align 2 {
  %6 = alloca %class.StringBuffer*, align 8
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i8*, align 8
  %10 = alloca i32, align 4
  store %class.StringBuffer* %0, %class.StringBuffer** %6, align 8
  store i32 %1, i32* %7, align 4
  store i32 %2, i32* %8, align 4
  store i8* %3, i8** %9, align 8
  store i32 %4, i32* %10, align 4
  %11 = load %class.StringBuffer*, %class.StringBuffer** %6, align 8
  %12 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %11, i32 0, i32 3
  %13 = call i32 @pthread_mutex_lock(%union.pthread_mutex_t* %12) #14
  %14 = load i32, i32* %7, align 4
  %15 = icmp slt i32 %14, 0
  br i1 %15, label %16, label %17

16:                                               ; preds = %5
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.7, i64 0, i64 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.8, i64 0, i64 0), i32 63, i8* getelementptr inbounds ([51 x i8], [51 x i8]* @__PRETTY_FUNCTION__._ZN12StringBuffer8getCharsEiiPci, i64 0, i64 0)) #15
  unreachable

17:                                               ; preds = %5
  %18 = load i32, i32* %8, align 4
  %19 = icmp slt i32 %18, 0
  br i1 %19, label %25, label %20

20:                                               ; preds = %17
  %21 = load i32, i32* %8, align 4
  %22 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %11, i32 0, i32 2
  %23 = load i32, i32* %22, align 4
  %24 = icmp sgt i32 %21, %23
  br i1 %24, label %25, label %26

25:                                               ; preds = %20, %17
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.7, i64 0, i64 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.8, i64 0, i64 0), i32 66, i8* getelementptr inbounds ([51 x i8], [51 x i8]* @__PRETTY_FUNCTION__._ZN12StringBuffer8getCharsEiiPci, i64 0, i64 0)) #15
  unreachable

26:                                               ; preds = %20
  %27 = load i32, i32* %7, align 4
  %28 = load i32, i32* %8, align 4
  %29 = icmp sgt i32 %27, %28
  br i1 %29, label %30, label %31

30:                                               ; preds = %26
  call void @__assert_fail(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.7, i64 0, i64 0), i8* getelementptr inbounds ([16 x i8], [16 x i8]* @.str.8, i64 0, i64 0), i32 69, i8* getelementptr inbounds ([51 x i8], [51 x i8]* @__PRETTY_FUNCTION__._ZN12StringBuffer8getCharsEiiPci, i64 0, i64 0)) #15
  unreachable

31:                                               ; preds = %26
  %32 = load i8*, i8** %9, align 8
  %33 = load i32, i32* %10, align 4
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds i8, i8* %32, i64 %34
  %36 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %11, i32 0, i32 0
  %37 = load i8*, i8** %36, align 8
  %38 = load i32, i32* %7, align 4
  %39 = sext i32 %38 to i64
  %40 = getelementptr inbounds i8, i8* %37, i64 %39
  %41 = load i32, i32* %8, align 4
  %42 = load i32, i32* %7, align 4
  %43 = sub nsw i32 %41, %42
  %44 = sext i32 %43 to i64
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 1 %35, i8* align 1 %40, i64 %44, i1 false)
  %45 = getelementptr inbounds %class.StringBuffer, %class.StringBuffer* %11, i32 0, i32 3
  %46 = call i32 @pthread_mutex_unlock(%union.pthread_mutex_t* %45) #14
  ret void
}

; Function Attrs: noinline uwtable
define internal void @_GLOBAL__sub_I_stringbuffer.cc() #0 section ".text.startup" {
  call void @__cxx_global_var_init()
  call void @__cxx_global_var_init.1()
  ret void
}

attributes #0 = { noinline uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nobuiltin allocsize(0) "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noinline optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nobuiltin nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #8 = { nounwind readonly "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #9 = { argmemonly nounwind willreturn }
attributes #10 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #11 = { builtin allocsize(0) }
attributes #12 = { builtin nounwind }
attributes #13 = { nounwind readonly }
attributes #14 = { nounwind }
attributes #15 = { noreturn nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1 (https://github.com/llvm/llvm-project.git 43ff75f2c3feef64f9d73328230d34dac8832a91)"}
