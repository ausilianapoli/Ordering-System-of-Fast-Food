<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$tavolo = mysqli_real_escape_string($conn, $_POST['ID_Tavolo']);

		$sql = "DELETE FROM tavolo WHERE ID_Tavolo='$tavolo';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>