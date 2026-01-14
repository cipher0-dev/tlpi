
function test_a() {
  local test_file="build/15-1-test_a"

  rm -f $test_file 2>/dev/null

  echo
  echo "Create new file with 666"
  mkdir build
  echo foobar > $test_file
  chmod 666 $test_file
  ls -la $test_file

  echo
  echo "Remove ownership bits"
  chmod u= $test_file
  ls -la $test_file

  echo
  echo "This should fail"
  echo bazqux >> $test_file
}

function test_b() {
  local test_dir="build/15-1-test_b"

  if [ -d $test_dir ]; then
    echo
    echo "Need sudo to cleanup previous run"
    sudo rm -rf $test_dir
  fi

  echo
  echo "Create new dir with 755"
  mkdir -p $test_dir
  echo foo > $test_dir/foo
  cat $test_dir/foo
  ls -lag $test_dir

  echo
  echo "Remove execute perms"
  chmod -x $test_dir

  echo
  echo "This should pass"
  ls $test_dir

  echo
  echo "This should fail"
  cat $test_dir/foo
}

function main() {
  test_a
  test_b
}

main
