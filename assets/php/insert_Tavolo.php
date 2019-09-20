<?php

	include_once 'dbh.php';

	$tavolo = mysqli_real_escape_string($conn, $_POST['ID_Tavolo']);

	$sql = "INSERT INTO tavolo VALUES (?);";
	$stmt = mysqli_stmt_init($conn);
	if(!mysqli_stmt_prepare($stmt, $sql)){
		echo "SQL Error";
	}
	else{
		mysqli_stmt_bind_param($stmt, "s", $tavolo);
		mysqli_stmt_execute($stmt);
	}

	header("Location: ../php/success.php");

?>