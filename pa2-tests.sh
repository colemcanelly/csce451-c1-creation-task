#!/usr/bin/env bash

# function to clean up files and make executables
remake () {
    #printf "\nCleaning old files and making executables\n"
    make -s clean
    make -s >/dev/null 2>&1
}


printf "To remove colour from tests, set COLOUR to 1 in sh file\n\n"
COLOUR=0
if [[ COLOUR -eq 0 ]]; then
    BOLD='\033[1m'
    UNDERLINE='\033[4m'
    NOUNDERLINE='\033[24m'
    INVERT='\033[7m'
    NOINVERT='\033[27m'
    ORANGE='\033[0;33m'
    GREEN='\033[0;32m'
    RED='\033[0;31m'
    NC='\033[0m'
else
    BOLD='\033[0m'
    UNDERLINE='\033[0m'
    NOUNDERLINE='\033[0m'
    INVERT='\033[0m'
    ORANGE='\033[0m'
    GREEN='\033[0m'
    RED='\033[0m'
    NC='\033[0m'
fi

all_passed=true
all_failed=true
SCORE=0
MAX=0


#printf "\nStart testing\n"
test_echo() {
    local PTS=2
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: echo \"Hello world | Life is Good > Great $\"\r"
    cat ./test-files/test_echo_double.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES=$(. ./test-files/test_echo_double.txt)
    if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
}

test_ls_default() {
    local PTS=2
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: ls\r"
    cat ./test-files/test_ls.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES=$(. ./test-files/test_ls.txt)
    if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
}

test_ls_l_usr() {
    local PTS=2
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: ls -l /usr/bin\r"
    cat ./test-files/test_ls_l_usr_bin.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES=$(. ./test-files/test_ls_l_usr_bin.txt)
    if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
}

test_ls_al() {
    local PTS=2
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: ls -l -a\r"
    cat ./test-files/test_ls_l_a.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES=$(. ./test-files/test_ls_l_a.txt)
    if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
}

test_ps() {
    local PTS=2
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: ps aux\r"
    cat ./test-files/test_ps_aux.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES=$(. ./test-files/test_ps_aux.txt)
    if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
}

test_io_redirect() {
    local PTS=12
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: ps aux > a; grep /init < a; grep /init < a > b\r"
    cat ./test-files/test_input_output_redirection.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES=$(. ./test-files/test_input_output_redirection.txt)
    rm -f a b
    ./shell < ./test-files/cmd.txt >temp 2>/dev/null
    if grep -qF -- "${RES}" temp; then
        if [ -f a ] && [ -f b ] && grep -qF -- "${RES}" b; then
            printf "    ${GREEN}Passed${NC}\n"
            SCORE=$(($SCORE+$PTS))
            all_failed=false
        else
            printf "    ${RED}Failed file creation${NC}\n"
        fi
    else
        printf "    ${RED}Failed final output${NC}\n"
    fi
    rm temp
}

test_pipe_single() {
    local PTS=6
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: ls -l | grep \"shell.cpp\"\r"
    cat ./test-files/test_single_pipe.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES=$(. ./test-files/test_single_pipe.txt)
    NOTRES=$(ls -l | grep "Tokenizer.cpp")
    strace -e trace=execve -f -o out.trace ./shell < ./test-files/cmd.txt >temp 2>/dev/null
    LS=$(which ls)
    GREP=$(which grep)
    if grep -q "execve(\"${LS}\"" out.trace && grep -q "execve(\"${GREP}\"" out.trace && grep -qFw -- "${RES}" temp && ! grep -qFw -- "${NOTRES}" temp; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
    rm temp
}

test_pipes() {
    local PTS=4
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: ps aux | awk ""'""/usr/{print \$1}""'"" | sort -r\r"
    cat ./test-files/test_multiple_pipes_A.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES=$(. ./test-files/test_multiple_pipes_A.txt)
    ARR=($RES)
    echo "${RES}" >cnt.txt
    CNT=$(grep -oF -- "${ARR[0]}" cnt.txt | wc -l)
    strace -e trace=execve -f -o out.trace ./shell < ./test-files/cmd.txt >temp 2>/dev/null
    PS=$(which ps)
    AWK=$(which awk)
    SORT=$(which sort)
    if grep -q "execve(\"${PS}\"" out.trace && grep -q "execve(\"${AWK}\"" out.trace && grep -q "execve(\"${SORT}\"" out.trace && grep -qFw -- "${RES}" temp && [ $(grep -oFw -- "${ARR[0]}" temp | wc -l) -le $((${CNT}+3)) ]; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
    rm cnt.txt temp
}

test_pipes_io() {
    local PTS=12
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: Multiple Pipes & Redirection\r"
    cat ./test-files/test_multiple_pipes_redirection.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES=$(. ./test-files/test_multiple_pipes_redirection.txt)
    echo "${RES}" >cnt.txt
    CNT=$(grep -oF -- "${RES}" cnt.txt | wc -l)
    rm -f cnt.txt test.txt output.txt
    if [ $(./shell < ./test-files/cmd.txt 2>/dev/null | grep -oF -- "${RES}" | wc -l) -eq ${CNT} ] && [ -f test.txt ] && [ -f output.txt ]; then 
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
}

test_cdA() {
    local PTS=3
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: cd ../../\r"
    cat ./test-files/test_cd_A.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    DIR=$(. ./test-files/test_cd_A.txt)
    ./shell < ./test-files/cmd.txt >temp 2>/dev/null
    if [ $(grep -oF -- "${DIR}" temp | wc -l) -ge 3 ] && [ $(grep -oF -- "${DIR}/" temp | wc -l) -le 1 ]; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
    rm temp
}

test_cdB() {
    local PTS=3
    MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: cd -\r"
    cat ./test-files/test_cd_B.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    TEMPDIR=$(cd /home && pwd)
    DIR=$(. ./test-files/test_cd_B.txt | head -n 1)
    ./shell < ./test-files/cmd.txt >temp 2>/dev/null
    if [ $(grep -oF -- "${DIR}" temp | wc -l) -ge 3 ] && ( [ $(grep -oF -- "${TEMPDIR}" temp | wc -l) -le 1 ] || ( grep -qF -- "${TEMPDIR}/" <<< "$DIR" && [ $(grep -oF -- "${TEMPDIR}" temp | wc -l) -gt $(grep -oF -- "${DIR}" temp | wc -l) ] ) ); then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
    rm temp
}

test_se_single() {
    local PTS=5
    # MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: Signular Sign Expansion\r"
    cat ./test-files/test_se_single.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES1=$(ps|grep bash|head -1|awk '{print $1}')
    RES=$(cat /proc/${RES1}/status)
    echo "${RES}" >cnt.txt
    if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
    rm -f cnt.txt
}

test_se_multiple() {
    local PTS=5
    # MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: Multiple Sign Expansions\r"
    cat ./test-files/test_se_many.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES1=$(ps|grep bash|head -1|awk '{print $1}')
    RES=$(cat /proc/${RES1}/status)
    echo "${RES}" >cnt.txt
    if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
    rm -f cnt.txt
}

test_se_nested() {
    local PTS=5
    # MAX=$(($MAX+$PTS))
    remake
    printf "\n\t\tTesting :: Nested Sign Expansions\r"
    cat ./test-files/test_se_nested.txt ./test-files/test_exit.txt > ./test-files/cmd.txt
    RES1=$(ps|grep bash|head -1|awk '{print $1}')
    RES=$(cat /proc/${RES1}/status)
    echo "${RES}" >cnt.txt
    if ./shell < ./test-files/cmd.txt 2>/dev/null | grep -qF -- "${RES}"; then
        printf "    ${GREEN}Passed${NC}\n"
        SCORE=$(($SCORE+$PTS))
        all_failed=false
    else
        printf "    ${RED}Failed${NC}\n"
        all_passed=false
    fi
    rm -f cnt.txt
}

test_ls() {
    test_ls_default
    test_ls_l_usr
    test_ls_al
}

test_cd() {
    test_cdA
    test_cdB
}

test_io() {
    test_io_redirect
    test_pipes_io
}

test_pipe() {
    test_pipe_single
    test_pipes
    test_pipes_io    
}

test_se() {
    test_se_single
    test_se_multiple
    test_se_nested
}

test_all() {
    SCORE=$(($SCORE+5))     # Background processes
    MAX=$(($MAX+5))
    SCORE=$(($SCORE+5))     # User prompt
    MAX=$(($MAX+5))
    
    test_echo               # Single Test
    test_ps                 # Single Test

    test_ls                 # Group Test
    test_io_redirect        # Single Test
    test_pipe               # Group Test
    test_cd                 # Group Test
    test_se                 # Group Test
}

if [ $# -eq 0 ]; then
    test_all
else
    for arg in $@
    do
        test_$arg
    done
fi

printf "\n\n\t"
if [ "$all_passed" = true ] ; then
    printf "${GREEN}${BOLD}${UNDERLINE}${INVERT}All tests Passed"
elif [ "$all_failed" = true ]; then
    printf "${RED}${BOLD}${UNDERLINE}${INVERT}All tests Failed"
else
    printf "${RED}${BOLD}${UNDERLINE}Some tests Failed"
fi

printf "${NOUNDERLINE}${NOINVERT}\t"
if [ ${SCORE} -eq ${MAX} ]; then
    printf "${NC}${SCORE}"
else
    printf "${SCORE}"
fi
printf "${NC}/${MAX}"

printf "${NC}\n\n\n"

exit 0