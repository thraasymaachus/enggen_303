log_files = {'putty2.log', 'putty4.log', 'putty8.log', 'putty16.log', 'putty32.log', 'putty64.log', 'putty128.log'};
prescale = [2, 4, 8, 16, 32, 64, 128];
n_files = length(log_files);

v_ref = 5;
N = 10;
lsb = v_ref/(2^N - 1);

gain_error = zeros(1, n_files);

% Initialize trendlines cell array
trendlines = cell(1, n_files);

for i = 1:n_files
    % Read data from file
    data = dlmread(log_files{i}, '', 1, 0);
    
    % Adjust phase
    data = make_data_in_phase(data);

    % Scale data
    scaled_data = data * (1023/255);

    % Fit a trendline passing through the origin
    V = linspace(0, 5, length(scaled_data))';
    m = sum(V .* scaled_data) / sum(V .* V); % Calculate the slope with transposed V and scaled_data
    trendline = m * V'; % Calculate the linear trendline

    % Store the trendline in the cell array
    trendlines{i} = trendline;

    % Find the first point where the difference between the trendline and (1023/5)*V is greater than 1.5*lsb
    threshold = 1.5;
    diff_data = (1023/5)*V' - trendline;
    index = find(diff_data > threshold, 1);

    % Calculate the gain error
    x_coord = V(index);
    y_coord = trendline(index);
    x_target = interp1((1023/5)*V', V', y_coord, 'linear');
    gain_error(i) = x_coord - x_target;
end



% Display gain error
fprintf('Prescale\tGain Error\n');
for i = 1:n_files
    fprintf('%d\t\t\t%.2fmV\n', prescale(i), 1000*gain_error(i));
end

% Call the plot_trendlines function
plot_trendlines(trendlines, V', n_files, prescale);

% Function to remove offset error by starting from the last zero and wrapping around
function data = make_data_in_phase(data)
    % Find the indices of all zeros in the data
    zero_indices = find(data == 0);
    
    % Find the index of the last zero in the data
    last_zero_idx = zero_indices(end);
    
    % Reorder the data to start from the last zero and wrap around
    data = [data(last_zero_idx:end); data(1:last_zero_idx-1)];
end

% Function to plot the trendlines
function plot_trendlines(trendlines, V, n_files, prescale)
    figure;
    hold on;
    
    for i = 1:n_files
        plot(V, trendlines{i}, 'DisplayName', ['Prescale ' num2str(prescale(i))]);
    end
 
    plot(V, (1023/5)*V, 'k--', 'DisplayName', 'Ideal');
    
    xlabel('Vin');
    ylabel('Output');
    legend('show');
    hold off;
end