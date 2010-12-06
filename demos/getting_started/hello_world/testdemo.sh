#/bin/sh

# collect all of the executables that start with "server*"
ALLSERVER="server*"
for s in $ALLSERVER 
  do
  if [ -x $s ]
    then
    SERVERS="$SERVERS $s"
  fi
done

echo "Starting servers..."
# Start the servers
for server in $SERVERS
  do
  ./$server 1> $server.output.tmp &
  PIDS="$PIDS $!"
done

sleep 2

echo "Starting Client..."
#Start the client
./client 1> client.output.tmp &
PIDS="$PIDS $!"

sleep 5
jobs
echo "Killing processes..."
#terminate the processes
for p in $PIDS
  do
  kill $p
done

jobs

# Now, compare the acquired outputs with the expected outputs
for server in $SERVERS
  do
  if diff $server.output.tmp $server.output > /dev/null 
    then
    echo "."
    exit 0
    else
    echo "ERROR: Files Differ: $server.output $server.output.tmp"
    exit 1
  fi
done

if diff client.output.tmp client.output > /dev/null 
  then
  echo "."
  exit 0
  else
  echo "ERROR: Files Differ: client.output client.output.tmp"
  exit 1
fi

# Remove tmp files
for server in $SERVERS
  do
  rm $server.output.tmp
done
rm client.output.tmp
