###
# dotnet of swig
#

DOTNET_ALL		= ${DOTNET_MANAGED_DLL}					\
				${DOTNET_UNMANAGED_DLL}				\
				${DOTNET_DEMO_EXE}

DOTNET_BUILD		= ${DOTNET_WRAP}  ${DOTNET_ALL}
DOTNET_CXX_FLAGS	= -fPIC `${SWIG_LIBRETS_CONFIG} --cflags`
DOTNET_DEMO_EXE		= ${DOTNET_GETOBJECT_EXE}				\
				${DOTNET_INTERLEAVED_EXE}			\
				${DOTNET_METADATA_EXE}				\
				${DOTNET_LOGGING_EXE}				\
				${DOTNET_LOGIN_EXE}				\
				${DOTNET_RAWSEARCH_EXE}				\
				${DOTNET_SEARCH_EXE}                            \
                                ${DOTNET_SQL2DMQL_EXE}                          \
                                ${DOTNET_UPDATE_EXE}
DOTNET_DEMO_SRC		= ${DOTNET_GETOBJECT_SRC}				\
				${DOTNET_INTERLEAVED_SRC}			\
				${DOTNET_METADATA_SRC}				\
				${DOTNET_LOGGING_SRC}				\
				${DOTNET_LOGIN_SRC}				\
				${DOTNET_RAWSEARCH_SRC}				\
				${DOTNET_SEARCH_SRC}				\
                                ${DOTNET_SQL2DMQL_SRC}                          \
                                ${DOTNET_UPDATE_SRC}
DOTNET_DIR		= ${SWIG_DIR}/csharp
DOTNET_GENERATED_SRC	= ${wildcard ${DOTNET_OBJ_DIR}/*.cs}
DOTNET_GETOBJECT_EXE	= ${DOTNET_OBJ_DIR}/GetObject.exe
DOTNET_GETOBJECT_SRC	= ${DOTNET_DIR}/GetObject.cs
DOTNET_INSTALL		= DOTNET_install
DOTNET_INTERLEAVED_EXE	= ${DOTNET_OBJ_DIR}/Interleaved.exe
DOTNET_INTERLEAVED_SRC	= ${DOTNET_DIR}/Interleaved.cs ${DOTNET_DIR}/Options.cs
DOTNET_LOGGING_EXE	= ${DOTNET_OBJ_DIR}/Logging.exe
DOTNET_LOGGING_SRC	= ${DOTNET_DIR}/Logging.cs
DOTNET_LOGIN_EXE	= ${DOTNET_OBJ_DIR}/Login.exe
DOTNET_LOGIN_SRC	= ${DOTNET_DIR}/Login.cs
DOTNET_MANAGED_DLL	= ${DOTNET_OBJ_DIR}/librets-dotnet.dll
DOTNET_MANAGED_SRC	= ${DOTNET_GENERATED_SRC}		 		\
				${DOTNET_DIR}/CppInputStream.cs 		\
        			${DOTNET_DIR}/ObjectDescriptorEnumerator.cs 	\
				${DOTNET_DIR}/TextWriterLogger.cs		\
				${DOTNET_DIR}/RetsExceptionNative.cs		\
				${DOTNET_DIR}/RetsReplyExceptionNative.cs	\
				${DOTNET_DIR}/RetsHttpExceptionNative.cs

DOTNET_METADATA_EXE	= ${DOTNET_OBJ_DIR}/Metadata.exe
DOTNET_METADATA_SRC	= ${DOTNET_DIR}/Metadata.cs ${DOTNET_DIR}/Options.cs
DOTNET_OBJ_DIR		= ${SWIG_OBJ_DIR}/csharp
DOTNET_PLATFORM		= 
DOTNET_RAWSEARCH_EXE	= ${DOTNET_OBJ_DIR}/RawSearch.exe
DOTNET_RAWSEARCH_SRC	= ${DOTNET_DIR}/RawSearch.cs ${DOTNET_DIR}/Options.cs
DOTNET_SEARCH_EXE	= ${DOTNET_OBJ_DIR}/Search.exe
DOTNET_SEARCH_SRC	= ${DOTNET_DIR}/Search.cs ${DOTNET_DIR}/Options.cs
DOTNET_SQL2DMQL_EXE	= ${DOTNET_OBJ_DIR}/Sql2DMQL.exe
DOTNET_SQL2DMQL_SRC	= ${DOTNET_DIR}/Sql2DMQL.cs ${DOTNET_DIR}/SimpleSqlMetadata.cs
DOTNET_UNMANAGED_DLL	= ${DOTNET_OBJ_DIR}/librets.${DLL}

ifeq (${SWIG_OSNAME}, MSWin32)
DOTNET_UNMANAGED_DLL	= ${DOTNET_OBJ_DIR}/librets.${DLL}
endif

DOTNET_UNMANAGED_OBJ	= ${DOTNET_OBJ_DIR}/librets_wrap.o 			\
				${DOTNET_OBJ_DIR}/librets_sharp.o		\
				${SWIG_BRIDGE_OBJ}
DOTNET_UPDATE_EXE	= ${DOTNET_OBJ_DIR}/Update.exe
DOTNET_UPDATE_SRC	= ${DOTNET_DIR}/Update.cs ${DOTNET_DIR}/Options.cs
DOTNET_WRAP		= ${DOTNET_OBJ_DIR}/librets_wrap.cpp


${DOTNET_WRAP}: ${SWIG_FILES} 
	${SWIG} -c++ -csharp -namespace librets -o ${DOTNET_WRAP} \
	-outdir ${DOTNET_OBJ_DIR} -I${SWIG_DIR}/lib/csharp ${SWIG_DIR}/librets.i
	${MAKE} ${DOTNET_MANAGED_DLL}

${DOTNET_UNMANAGED_DLL}: ${DOTNET_UNMANAGED_OBJ} ${LIBRETS_LIB}
	${SWIG_LINK} -o ${DOTNET_UNMANAGED_DLL} ${DOTNET_UNMANAGED_OBJ} ${SWIG_LIBRETS_LIBS} 

${DOTNET_OBJ_DIR}/%.o: ${DOTNET_OBJ_DIR}/%.cpp 
	${CXX} ${DOTNET_CXX_FLAGS} -I${LIBRETS_INC_DIR} -I${DOTNET_OBJ_DIR} -I${DOTNET_DIR} -I${SWIG_DIR} -c $< -o $@

${DOTNET_OBJ_DIR}/%.o: ${DOTNET_DIR}/%.cpp
	${CXX} ${DOTNET_CXX_FLAGS} -I${LIBRETS_INC_DIR} -I${DOTNET_OBJ_DIR} -I${DOTNET_DIR} -I${SWIG_DIR} -c $< -o $@

ifneq (${SWIG_OSNAME}, MSWin32)
########
#
# Not Windows/MinGW
#

${DOTNET_MANAGED_DLL}:	${DOTNET_UNMANAGED_DLL} ${DOTNET_MANAGED_SRC}
	${MCS} -target:library -out:${DOTNET_MANAGED_DLL} ${DOTNET_PLATFORM} ${DOTNET_MANAGED_SRC}

${DOTNET_GETOBJECT_EXE}:	${DOTNET_GETOBJECT_SRC}
	${MCS} -r:${DOTNET_MANAGED_DLL} ${DOTNET_PLATFORM} -out:${DOTNET_GETOBJECT_EXE} ${DOTNET_GETOBJECT_SRC}

${DOTNET_INTERLEAVED_EXE}:	${DOTNET_INTERLEAVED_SRC}
	${MCS} -r:${DOTNET_MANAGED_DLL} ${DOTNET_PLATFORM} -out:${DOTNET_INTERLEAVED_EXE}  ${DOTNET_INTERLEAVED_SRC}

${DOTNET_METADATA_EXE}:		${DOTNET_METADATA_SRC}
	${MCS} -r:${DOTNET_MANAGED_DLL} ${DOTNET_PLATFORM} -out:${DOTNET_METADATA_EXE} ${DOTNET_METADATA_SRC}

${DOTNET_LOGGING_EXE}:		${DOTNET_LOGGING_SRC}
	${MCS} -r:${DOTNET_MANAGED_DLL} ${DOTNET_PLATFORM} -out:${DOTNET_LOGGING_EXE}  ${DOTNET_LOGGING_SRC}

${DOTNET_LOGIN_EXE}:		${DOTNET_LOGIN_SRC}
	${MCS} -r:${DOTNET_MANAGED_DLL} ${DOTNET_PLATFORM} -out:${DOTNET_LOGIN_EXE}  ${DOTNET_LOGIN_SRC}

${DOTNET_RAWSEARCH_EXE}:	${DOTNET_RAWSEARCH_SRC}
	${MCS} -r:${DOTNET_MANAGED_DLL} ${DOTNET_PLATFORM} -out:${DOTNET_RAWSEARCH_EXE}  ${DOTNET_RAWSEARCH_SRC}

${DOTNET_SEARCH_EXE}:		${DOTNET_SEARCH_SRC}
	${MCS} -r:${DOTNET_MANAGED_DLL} ${DOTNET_PLATFORM} -out:${DOTNET_SEARCH_EXE}  ${DOTNET_SEARCH_SRC}

${DOTNET_SQL2DMQL_EXE}:		${DOTNET_SQL2DMQL_SRC}
	${MCS} -r:${DOTNET_MANAGED_DLL} -main:Sql2DMQL ${DOTNET_PLATFORM} -out:${DOTNET_SQL2DMQL_EXE} ${DOTNET_SQL2DMQL_SRC}

${DOTNET_UPDATE_EXE}:	        ${DOTNET_UPDATE_SRC}
	${MCS} -r:${DOTNET_MANAGED_DLL} ${DOTNET_PLATFORM} -out:${DOTNET_UPDATE_EXE}  ${DOTNET_UPDATE_SRC}

else
########
#
# Windows/MinGW
#
BACKSLASH	= perl -e 'foreach (<STDIN>) {s/\//\\/g; print}'

${DOTNET_MANAGED_DLL}:	${DOTNET_UNMANAGED_DLL} ${DOTNET_MANAGED_SRC}
	${CSC} -target:library \
		-platform:${TARGET_CPU} \
                -out:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
                ${shell echo ${DOTNET_PLATFORM} | ${BACKSLASH}} \
                ${shell echo ${DOTNET_MANAGED_SRC} | ${BACKSLASH}}

${DOTNET_GETOBJECT_EXE}:	${DOTNET_GETOBJECT_SRC}
	${CSC} -r:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
		-platform:${TARGET_CPU} \
		-out:${shell echo ${DOTNET_GETOBJECT_EXE} | ${BACKSLASH}} \
		${shell echo ${DOTNET_GETOBJECT_SRC} | ${BACKSLASH}}

${DOTNET_INTERLEAVED_EXE}:	${DOTNET_INTERLEAVED_SRC}
	${CSC} -r:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
		-platform:${TARGET_CPU} \
		-out:${shell echo ${DOTNET_INTERLEAVED_EXE} | ${BACKSLASH}} \
		${shell echo ${DOTNET_INTERLEAVED_SRC} | ${BACKSLASH}}

${DOTNET_METADATA_EXE}:		${DOTNET_METADATA_SRC}
	${CSC} -r:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
		-platform:${TARGET_CPU} \
		-out:${shell echo ${DOTNET_METADATA_EXE} | ${BACKSLASH}} \
		${shell echo ${DOTNET_METADATA_SRC} | ${BACKSLASH}}

${DOTNET_LOGGING_EXE}:		${DOTNET_LOGGING_SRC}
	${CSC} -r:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
		-platform:${TARGET_CPU} \
		-out:${shell echo ${DOTNET_LOGGING_EXE} | ${BACKSLASH}} \
		${shell echo ${DOTNET_LOGGING_SRC} | ${BACKSLASH}}

${DOTNET_LOGIN_EXE}:		${DOTNET_LOGIN_SRC}
	${CSC} -r:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
		-platform:${TARGET_CPU} \
		-out:${shell echo ${DOTNET_LOGIN_EXE} | ${BACKSLASH}} \
		${shell echo ${DOTNET_LOGIN_SRC} | ${BACKSLASH}}

${DOTNET_RAWSEARCH_EXE}:	${DOTNET_RAWSEARCH_SRC}
	${CSC} -r:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
		-platform:${TARGET_CPU} \
		-out:${shell echo ${DOTNET_RAWSEARCH_EXE} | ${BACKSLASH}} \
		${shell echo ${DOTNET_RAWSEARCH_SRC} | ${BACKSLASH}}

${DOTNET_SEARCH_EXE}:		${DOTNET_SEARCH_SRC}
	${CSC} -r:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
		-platform:${TARGET_CPU} \
		-out:${shell echo ${DOTNET_SEARCH_EXE} | ${BACKSLASH}} \
		${shell echo ${DOTNET_SEARCH_SRC} | ${BACKSLASH}}

${DOTNET_SQL2DMQL_EXE}:		${DOTNET_SQL2DMQL_SRC}
	${CSC} -r:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
		-platform:${TARGET_CPU} \
		-main:Sql2DMQL ${DOTNET_PLATFORM} \
		-out:${shell echo ${DOTNET_SQL2DMQL_EXE} | ${BACKSLASH}} \
		${shell echo ${DOTNET_SQL2DMQL_SRC} | ${BACKSLASH}}

${DOTNET_UPDATE_EXE}:	        ${DOTNET_UPDATE_SRC}
	${CSC} -r:${shell echo ${DOTNET_MANAGED_DLL} | ${BACKSLASH}} \
		-platform:${TARGET_CPU} \
		-out:${shell echo ${DOTNET_UPDATE_EXE} | ${BACKSLASH}} \
		${shell echo ${DOTNET_UPDATE_SRC} | ${BACKSLASH}}

endif
