<?php
  $error = "";
  $address = "";
  $rom = $_GET["rom"];

  // Make sure only 1 PHP script can access the start_game.py script
  // at one time.
  $fp = fopen("/tmp/cloudtari_lock.txt", "w");
  flock($fp, LOCK_EX);

  $handle = popen("/usr/bin/sudo -u ubuntu /var/www/html/start_game.py " . $rom, "r");

  if (!$handle)
  {
    $error = "Could not open launcher.";
  }

  while (($line = fgets($handle)) !== false)
  {
    $line = trim($line);

    if ($line == "server full")
    {
      $error = "Server Full";
    }
      else
    if (strpos($line, "address=") !== false)
    {
      $address = str_replace("address=", "", $line);
    }
  }

  pclose($handle);

  flock($fp, LOCK_UN);
  fclose($fp);

  if (strpos($address, "problem") !== false)
  {
    $error = "Kubernetes didn't start.";
  }

  if ($error == "")
  {
    header("Location: http://" . $address . "/");
    exit();
  }
?>
<html>
<body bgcolor="black" text="white">
<br><br><br>
<center>
<h1>
Problem launching game.
<br><br>
<?php print $error; ?>
</h1>
</center>
</body>
</html>

