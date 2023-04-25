% Add your log file names to the 'log_files' cell array
log_files = {'putty2.log', 'putty4.log', 'putty8.log', 'putty16.log', 'putty32.log', 'putty64.log', 'putty128.log'};

% Initialize a cell array to store the data from each log file
data = cell(1, numel(log_files));

% Read the log files and store the data in the 'data' cell array
for i = 1:numel(log_files)
    data{i} = read_log_file(log_files{i});
    data{i} = make_data_in_phase(data{i});
end

% Generate the time values for the sawtooth signal
t = 0:999;

% Create the reference ideal sawtooth signal
sawtooth_signal = 255/2 * (1 - sawtooth(2 * pi * t / 999, 0));

% Initialize arrays for Gain Error and INL
gain_errors = zeros(1, numel(log_files));
inl_values = zeros(1, numel(log_files));

% Calculate Gain Error and INL for each log file
for i = 1:numel(log_files)
    [gain_error, inl_value] = calculate_errors(data{i}, sawtooth_signal);
    gain_errors(i) = gain_error;
    inl_values(i) = inl_value;
end


% Plot Gain Error and INL
plot_errors(gain_errors, inl_values, log_files);

% Function to read log files and store the data in an array
function data = read_log_file(file_name)
    % Read the log file
    file_content = fileread(file_name);
    
    % Remove the header by finding the first newline character
    first_newline = find(file_content == newline, 1, 'first');
    file_content = file_content(first_newline+1:end);
    
    % Split the file content by newline characters
    file_lines = split(file_content, newline);
    
    % Convert the strings to integers
    data = str2double(file_lines);
end

% Function to make data in phase by starting from zero and wrapping around
function data = make_data_in_phase(data)
    % Find the index of the first zero in the data
    first_zero_idx = find(data == 0, 1, 'first');
    
    % Reorder the data to start from zero and wrap around
    data = [data(first_zero_idx:end); data(1:first_zero_idx-1)];
end

% Function to calculate Gain Error and INL
function [gain_error, inl] = calculate_errors(data, ideal_signal)
    % Calculate the Gain Error
    gain_error = (data(end) - data(1)) / (ideal_signal(end) - ideal_signal(1)) - 1;

    % Compensate for Gain Error
    data_compensated = data / (1 + gain_error);

    % Calculate the INL
    ideal_data = linspace(0, 999, 1000);
    inl = max(abs(data_compensated - ideal_data) / (255/999));
end

% Function to plot Gain Error and INL
function plot_errors(gain_errors, inl_values, log_files)
    % Plot Gain Errors
    figure
    bar(gain_errors)
    set(gca, 'XTickLabel', log_files)
    xlabel('Log Files')
    ylabel('Gain Error')
    title('Gain Errors')

    % Plot INL
    figure
    scatter(1:numel(log_files), inl_values)
    set(gca, 'XTick', 1:numel(log_files), 'XTickLabel', log_files)
    xlabel('Log Files')
    ylabel('INL (LSB)')
    title('Integral Non-linearity (INL)')
end
