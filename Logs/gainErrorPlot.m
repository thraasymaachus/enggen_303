log_files = {'putty2.log', 'putty4.log', 'putty8.log', 'putty16.log', 'putty32.log', 'putty64.log', 'putty128.log'};
prescale = [2, 4, 8, 16, 32, 64, 128];
n_files = length(log_files);

v_ref = 5;
N = 8;
lsb = v_ref/(2^N - 1);
v_trans = v_ref - 1.5*lsb;
ideal_transition_idx = 1000 * (v_trans / v_ref);

gain_error = zeros(1, n_files);

for i = 1:n_files
    % Read data from file
    data = dlmread(log_files{i}, '', 1, 0);
    
    % Adjust phase
    data = make_data_in_phase(data);
    
    % Set last transition value to the last value in the adjusted data
    last_transition_idx = find(data > 249, 1, 'first');
    
    % Calculate gain error
    gain_error(i) = last_transition_idx - ideal_transition_idx;
end

% Display gain error
fprintf('Prescale\tGain Error\n');
for i = 1:n_files
    fprintf('%d\t\t%.2f\n', prescale(i), gain_error(i));
end

% Function to make data in phase by starting from zero and wrapping around
function data = make_data_in_phase(data)
    % Find the index of the first zero in the data
    first_zero_idx = find(data == 0, 1, 'first');
    
    % Reorder the data to start from zero and wrap around
    data = [data(first_zero_idx:end); data(1:first_zero_idx-1)];
end
