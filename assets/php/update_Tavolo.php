<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$tavolo_old = mysqli_real_escape_string($conn, $_POST['ID_Tavolo_old']);
		$tavolo_new = mysqli_real_escape_string($conn, $_POST['ID_Tavolo_new']);

		$sql = "UPDATE tavolo SET ID_Tavolo='$tavolo_new' WHERE ID_Tavolo='$tavolo_old';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>