#!/bin/sh
# Testovaci skript pro studenty resici 1. ulohu IOS 2013. Public domain.

WDPREFIX=test.tmp

if [ -t 1 ]; then
    c_red=`tput setaf 1`
    c_green=`tput setaf 2`
    c_normal=`tput sgr0`
fi

die()
{
    echo "error: $@"
    exit 1
}

# msg text message ...
msg()
{
    rcode=$?
    if [ $rcode = 0 ]; then
        printf "%-60s ${c_green}pass$c_normal\n" "$*"
    else
        printf "%-60s ${c_red}FAIL$c_normal\n" "$*"
    fi
    return $rcode
}

# smoke_tst file
smoke_tst()
{
    [ -x "./$1" ]
    msg "smoke test ./$1"
}

checkout_tst()
{
    log=../../tst-checkout.log
    W=$WDPREFIX-checkout
    mkdir -p $W/shared 2>/dev/null || die "mkdir $W/shared"
    echo Hello >$W/shared/f1
    echo world >$W/shared/f2
    mkdir $W/exp 2>/dev/null || die "mkdir $W/exp"
    cd $W/exp

    ../../le-checkout.sh ../shared >$log 2>&1

    [ -d .le ]
    if msg "le-checkout.sh: .le exists"; then
        cmp -s ../shared/f1 .le/f1; msg "le-checkout.sh .le/f1"
        cmp -s ../shared/f2 .le/f2; msg "le-checkout.sh .le/f2"
        cmp -s ../shared/f1 ./f1; msg "le-checkout.sh ./f1"
        cmp -s ../shared/f2 ./f2; msg "le-checkout.sh ./f2"
        [ -f .le/.config ]
        if msg "le-checkout.sh .le/.config exists"; then
            grep -q "^projdir  *\.\./shared" .le/.config
            msg "le-checkout.sh projdir in .le/.config"
        fi
    fi
    cd ../..
    rm -rf $W
}

update_tst()
{
    log=../../tst-update.log
    W=$WDPREFIX-update
    mkdir -p $W/shared 2>/dev/null || die "mkdir $W/shared"
    echo Hello >$W/shared/f1
    echo world >$W/shared/f2
    echo same >$W/shared/f3
    printf '1\n2\n3\n4\n5\n6\n' >$W/shared/f4
    printf '1\n2\n3\n4\n5\n6\n' >$W/shared/f5
    mkdir $W/exp 2>/dev/null || die "mkdir $W/exp"
    cd $W/exp
    mkdir .le || dir "mkdir .le"
    cp ../shared/* .
    cp ../shared/* .le
    echo "projdir ../shared" >.le/.config
    echo append >>f1
    echo append >>../shared/f2
    c=`echo 0 - insert;cat ../shared/f4`
    echo "$c" >../shared/f4
    c=`cat f4;echo 7 append`
    echo "$c" >f4
    c=`cat ../shared/f5;echo 7 append;cat ../shared/f5`
    echo "$c" >../shared/f5
    c=`cat f5;echo 7 - append;cat f5`
    echo "$c" >f5
    c=`cat .le/f5 .le/f5`
    echo "$c" >.le/f5

    ../../le-update.sh >$log 2>&1

    grep -q "^M: f1$" $log && cmp -s ../shared/f1 .le/f1 && ! cmp -s .le/f1 f1
    msg "le-update.sh: modified"
    grep -q "^U: f2$" $log && cmp -s ../shared/f2 .le/f2 && cmp -s .le/f2 f2
    msg "le-update.sh: updated"
    grep -q "^\.: f3$" $log && cmp -s ../shared/f3 .le/f3 && cmp -s .le/f3 f3
    msg "le-update.sh: unchanged"
    grep -q "^M+: f4$" $log && cmp -s ../shared/f4 .le/f4 && ! cmp -s .le/f4 f4
    msg "le-update.sh: modified, patched"
    grep -q "^M!: f5 conflict!$" $log && ! cmp -s ../shared/f5 f5
    msg "le-update.sh: modified, conflict"
    [ `ls -A . .le|sort|wc -l` -eq 15 ]
    msg "le-update.sh: local temporary files"
    cd ../..
    rm -rf $W
}

revert_tst()
{
    log=../../tst-revert.log
    W=$WDPREFIX-revert
    mkdir -p $W/shared 2>/dev/null || die "mkdir $W/shared"
    echo Hello >$W/shared/f1
    echo world >$W/shared/f2
    echo same >$W/shared/f3
    printf '1\n2\n3\n4\n5\n6\n' >$W/shared/f4
    mkdir $W/exp 2>/dev/null || die "mkdir $W/exp"
    cd $W/exp
    mkdir .le || dir "mkdir .le"
    cp ../shared/* .
    cp ../shared/* .le
    echo "projdir ../shared" >.le/.config
    echo "ignore f(2|3)" >>.le/.config
    echo append >>f1
    echo append >>f2
    echo append >>f3

    ../../le-revert.sh >$log 2>&1

    ! grep -q append f1 && cmp -s ../shared/f1 f1 && cmp -s ./f1 f1 &&
    ! grep -q append f4 && cmp -s ../shared/f4 f4 && cmp -s ./f4 f4
    msg "le-revert.sh: reverted"
    grep -q append f2 && ! cmp -s f2 .le/f2 &&
    grep -q append f3 && ! cmp -s f3 .le/f3
    msg "le-revert.sh: ignored by .config"
    cd ../..
    rm -rf $W
}

diff_tst()
{
    log=../../tst-diff.log
    W=$WDPREFIX-diff
    mkdir -p $W/shared 2>/dev/null || die "mkdir $W/shared"
    echo Hello >$W/shared/f1
    echo world >$W/shared/f2
    echo same >$W/shared/f3
    printf '1\n2\n3\n4\n5\n6\n' >$W/shared/f4
    printf '1\n2\n3\n4\n5\n6\n' >$W/shared/f5
    mkdir $W/exp 2>/dev/null || die "mkdir $W/exp"
    cd $W/exp
    mkdir .le || dir "mkdir .le"
    cp ../shared/* .
    cp ../shared/* .le
    echo "projdir ../shared" >.le/.config
    echo append1 >>f1
    echo append2 >>../shared/f2
    c=`echo 0 insert4;cat ../shared/f4`
    echo "$c" >../shared/f4
    c=`cat f4;echo 7 append4`
    echo "$c" >f4
    c=`cat ../shared/f5;echo 7 append5`
    echo "$c" >../shared/f5
    c=`cat f5;echo 7 append5a`
    echo "$c" >f5
    
    ../../le-diff.sh >$log 2>&1

    grep -q '^+append1' $log &&
    grep -q '^-append2' $log &&
    grep -q '^-0 insert4' $log &&
    grep -q '^+7 append4' $log &&
    grep -q '^-7 append5' $log &&
    grep -q '^+7 append5a' $log
    msg "le-diff.sh: unified diff"
    [ `ls -A . .le|sort|wc -l` -eq 15 ]
    msg "le-diff.sh: local temporary files"
    cd ../..
    rm -rf $W
}

[ -n "$WDPREFIX" ] || die "NULL WDPREFIX"

smoke_tst le-checkout.sh && checkout_tst
smoke_tst le-update.sh && update_tst
smoke_tst le-revert.sh && revert_tst
smoke_tst le-diff.sh && diff_tst

true
