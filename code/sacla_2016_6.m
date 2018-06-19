
%for j=1:1
    run_number = 448539
    file_name = sprintf('%d.h5',run_number) %% change
%%run_info_1 = '/run_347463/run_info/start_tag_number' %%change
%%run_info_2 = '/run_347463/run_info/end_tag_number' %%change

    file = H5F.open (file_name, 'H5F_ACC_RDONLY', 'H5P_DEFAULT'); %%read only mode

    dset1_filename = sprintf('run_%d/event_info/tag_number_list', run_number)
    dset1 = H5D.open (file, dset1_filename);
    dataspace1 = H5D.get_space(dset1);
    [dim1,dim2] = H5S.get_simple_extent_dims(dataspace1);
    data1 = H5D.read(dset1, 'H5ML_DEFAULT', 'H5S_ALL', dataspace1, 'H5P_DEFAULT');
    %%data1(2,1)
    H5D.close (dset1); 
    H5S.close (dataspace1);


    image_num = dim2 %%change

    for i=1:dim2
        %% read CCD images
        tag_name = sprintf('/run_%d/detector_2d_assembled_1/tag_%d/detector_data',run_number, data1(i,1));%%change
        dset = H5D.open (file, tag_name);
        dataspace = H5D.get_space(dset);
        data = H5D.read(dset, 'H5ML_DEFAULT', 'H5S_ALL', dataspace, 'H5P_DEFAULT');
        H5D.close (dset); 
        H5S.close (dataspace);
	
    	%% make binary file of CCD images
        mkdir('bin_front');
        out_file_name = sprintf('bin_front/%d.bin', data1(i,1));%%change
        out_bin_file = fopen(out_file_name, 'w');
        fwrite(out_bin_file, data, 'float32');
        fclose(out_bin_file);
	
        clear data;
        end

    for i=1:dim2
        %% read CCD images
        tag_name = sprintf('/run_%d/detector_2d_1/tag_%d/detector_data',run_number, data1(i,1));%%change
        dset = H5D.open (file, tag_name);
        dataspace = H5D.get_space(dset);
        data = H5D.read(dset, 'H5ML_DEFAULT', 'H5S_ALL', dataspace, 'H5P_DEFAULT');
        H5D.close (dset);
        H5S.close (dataspace);

        %% make binary file of CCD images
        mkdir('bin_side');
        out_file_name = sprintf('bin_side/%d.bin', data1(i,1));%%change
        out_bin_file = fopen(out_file_name, 'w');
        fwrite(out_bin_file, data, 'float32');
        fclose(out_bin_file);

        clear data;
    end

    for i=1:dim2
        %% read CCD images
        tag_name = sprintf('/run_%d/detector_2d_2/tag_%d/detector_data', run_number, data1(i,1));%%change
        dset = H5D.open (file, tag_name);
        dataspace = H5D.get_space(dset);
        data = H5D.read(dset, 'H5ML_DEFAULT', 'H5S_ALL', dataspace, 'H5P_DEFAULT');
        H5D.close (dset);
        H5S.close (dataspace);

        %% make binary file of CCD images
        mkdir('bin_back');
        out_file_name = sprintf('bin_back/%d.bin', data1(i,1));%%change
        out_bin_file = fopen(out_file_name, 'w');
        fwrite(out_bin_file, data, 'float32');
        fclose(out_bin_file);

        clear data;
    end

H5F.close(file);
%end
test = 'program finished'
