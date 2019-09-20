<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$bevanda = mysqli_real_escape_string($conn, $_POST['ID_Bevanda']);

		$sql = "DELETE FROM bevanda WHERE ID_Bevanda='$bevanda';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>