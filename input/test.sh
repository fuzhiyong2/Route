
for((i=80;i<=800;i++));
do
	./lineExt 'boundary_'$i.'txt'
	check 'centerline_'$i.'txt'
done
