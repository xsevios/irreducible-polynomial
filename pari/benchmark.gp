default(parisize, 10000000000);

polynom_reader(str) = {
    str = strsplit(str, " ");
    str = str[2..length(str)];
    str = strjoin(str, ",");
    str = concat("[", str);
    str = concat(str, "]");
    str_coefs = eval(str);
    return(Pol(str_coefs));
}

dim_reader(str) = {
    str = strsplit(str, " ");
    str = str[1];
    return(eval(str));
}

read_file(file_name, ~input_list, reader) =
{
    listkill(input_list);
    file = fileopen(file_name);
    filereadstr(file);
    while(l = filereadstr(file), listput(input_list, reader(l)));
    fileclose(file);
}

read_grid(file_name) =
{
    file = fileopen(file_name);
    grid_size = filereadstr(file);
    fileclose(file);
    return(eval(grid_size));
}

run_benchmark(polynom, dim, poly_grid, ~factored, factor_method) =
{
    factored = List();
    elapsed_list = List();

    T = getwalltime();
    for (i = 1, length(polynom),
        if (factor_method == 1, listput(factored,  factormod(polynom[i], dim[i], 1)),
              factor_method == 2, listput(factored,  polisirreducible(polynom[i] * Mod(1, dim[i]))),
              printf("Unknown factor method %d", factor_method));
        if (i % poly_grid == 0,
            listput(elapsed_list, getwalltime() - T);
            printf("%.3f%% (%d/%d)\n", i / length(polynom) * 100.0, i, length(polynom));
            T = getwalltime();
        );
    );

    return(List([factored, elapsed_list]));
}

save_result(filename, ~data_list) =
{
    file = fileopen(filename, "w");

    for (i = 1, length(data_list),
        filewrite(file, Str(data_list[i]));
    );

    fileclose(file);
}

save_elapsed(filename, ~data_list, ~dim, poly_grid) =
{
    file = fileopen(filename, "w");
    str_buf = "";
    cur_dim = dim[1];

    for (i = 1, length(data_list),
        if (dim[i * poly_grid] != cur_dim,
            cur_dim = dim[i * poly_grid];
            filewrite(file, str_buf);
            str_buf = "";
        );
        str_buf = concat(str_buf, Str(data_list[i]));
        str_buf = concat(str_buf, " ");
    );
    cur_dim = dim[length(data_list) * poly_grid];
    filewrite(file, str_buf);
    str_buf = "";

    fileclose(file);
}

polynoms_file_name = "/Users/nikitalomov/Projects/irreducible-polynomial/cmake-build-release-remote-host/source/poly-generator/polynoms.txt"

poly_grid = 0
polynom = List()
dim = List()
factored = List()

print("Reading file...");

poly_grid = read_grid(polynoms_file_name);
read_file(polynoms_file_name, ~polynom, polynom_reader);
read_file(polynoms_file_name, ~dim, dim_reader);

print("File read ok");
print("Polynoms grid: ", poly_grid);
print("Polynoms count: ", length(polynom));

result_list = run_benchmark(polynom, dim, poly_grid, factored, 2);
save_elapsed("elapsed.txt", ~result_list[2], ~dim, poly_grid)
save_result("results.txt", ~result_list[1])
