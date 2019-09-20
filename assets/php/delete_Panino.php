<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$panino = mysqli_real_escape_string($conn, $_POST['ID_Panino']);

		$sql = "DELETE FROM panino WHERE ID_Panino='$panino';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>