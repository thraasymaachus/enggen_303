log_files = {'putty2.log', 'putty4.log', 'putty8.log', 'putty16.log', 'putty32.log', 'putty64.log', 'putty128.log'};
prescale = [2, 4, 8, 16, 32, 64, 128];
n_files = length(log_files);

v_ref = 5;
N = 8;
lsb = v_ref/(2^N - 1);

trendlines = cell(1, n_files);

gain_error = zeros(1, n_files);
m_ratio = zeros(1, n_files);
inl = zeros(1, n_files);

for i = 1:n_files
    % Read data from file
    data = dlmread(log_files{i}, '', 1, 0);
    
    % Adjust phase
    data = make_data_in_phase(data);

    % Slice off data after peak
    peak_val = max(data);
    peak_idx = find(data == peak_val, 1, "first");
    data = data(1:1:peak_idx);
    %fprintf("%d\n", length(data));
    
    V = linspace(0, 5, length(data))';

    m = sum(V .* data) / sum(V .* V); % Calculate the slope with transposed V and scaled_data
    trendline = m * V'; % Calculate the linear trendline

    trendline(peak_idx+1:1000) = trendline(peak_idx); % Saturate trendline after max is reached

    % Store the trendline and m in the cell array
    trendlines{i} = trendline;
    
    % Calculate gain error
    gain_error(i) = trendline(peak_idx) - 255;
    m_ratio(i) = m/(255/5);

    
    data =  data * (255/5) / m;  % Adjust the data to remove gain error

    diff = abs(data - (255/5)*V);
    inl(i) = max(diff);

end



% Display gain error
fprintf('Prescale\t\t\tGain Error (LSB)\t\tGain Error (slope ratio)\tINL\n');
for i = 1:n_files
    fprintf('%d\t\t\t\t\t%.1fLSB\t\t\t\t\t%.4f\t\t\t\t\t\t%.4f\n', prescale(i), gain_error(i), m_ratio(i), inl(i));
end

% Call the plot_trendlines function
plot_trendlines(trendlines, n_files, prescale);

% plot_gain_error(gain_error, prescale, "LSB", "Gain Error in LSB as Prescalar is Varied (Log Graph)");

plot_gain_error(m_ratio, prescale, "Slope ratio", "Ratio of Data Trendline Slope to Ideal Slope as Prescalar is Varied (Log Graph)");

plot_gain_error(inl, prescale, "INL", "INL as Prescalar is Varied (Log Graph)")

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
function plot_trendlines(trendlines, n_files, prescale)
    figure;
    hold on;
    
    for i = 1:n_files
        plot(linspace(0, 5, 1000)', trendlines{i}, 'DisplayName', ['Prescale ' num2str(prescale(i))]);
    end

    plot(linspace(0, 5, 1000)', (255/5)*linspace(0, 5, 1000), 'DisplayName', ['Prescale ' num2str(prescale(i))]);
    
    xlabel('Vin');
    ylabel('Output');
    legend('show');
    hold off;
end

function plot_gain_error(gain_error, prescale, type, title_text)
    figure;
    hold on;

    scatter(log2(prescale), gain_error);

    p = polyfit(log2(prescale), gain_error, 1);
    fprintf("%.6fx + %.3f\n", p(1), p(2));
    x = linspace(1, length(prescale));
    plot(x, polyval(p, x));

    % Show R^2 correlation value
    c_val = corrcoef(log2(prescale), gain_error);
    r_val = c_val(2);
    subtitle(["R^2 = " num2str(r_val^2)]);

    xlabel('log2(Prescale)');
    ylabel(type);
    title(title_text);
    hold off;
end
