#test for various array size for 8 process
$result_8_x=""
For($i=100; $i -lt 10000000000; $i=$i*5)
{
    $tmp=mpiexec -np 8 Lab4.exe $i
    $result_8_x=$result_8_x+$tmp
    Write-Output $tmp
}
Write-Output $result_8_x

#test for various process count for fixed array size
$result_p_10000000=""
For($i=1; $i -lt 16; $i++)
{
    $tmp=mpiexec -np $i Lab4.exe 10000
    $result_p_10000000=$result_p_10000000+$tmp
    Write-Output $tmp
}
Write-Output $result_p_10000000