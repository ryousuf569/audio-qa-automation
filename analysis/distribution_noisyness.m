clear; clc;

results = readtable('C:\Users\ryous\Downloads\audio-qa-automation\data\processed\computation_results.csv');
stats = readtable('C:\Users\ryous\Downloads\audio-qa-automation\data\processed\stats.csv');

results.status_num = zeros(height(results),1);
results.status_num(strcmp(results.status,'OK'))   = 0;
results.status_num(strcmp(results.status,'WARN')) = 1;
results.status_num(strcmp(results.status,'FAIL')) = 2;

noise_results = results(strcmp(results.metric,'NoiseZCR'), :);
noise_data = join(stats, noise_results, 'Keys','file_id');

figure;
histogram(noise_data.zcr_p95, 30);
hold on;

xline(0.20, 'r', 'LineWidth', 2);

xlabel('Zero-Crossing Rate (95th Percentile)');
ylabel('Number of Files (1 By Default)');

title('Noise (ZCR) Distribution with Quality Threshold');

% Legend explains visual elements
legend('Observed ZCR Values', 'Max Allowed ZCR (0.25)', ...
       'Location','best');

grid on;

annotation('textbox', [0.60 0.70 0.35 0.18], ...
    'String', { ...
        'Interpretation:', ...
        'Low ZCR = clean signal', ...
        'High ZCR = noisy / harsh content', ...
        'Values near threshold = borderline quality', ...
        'Right-tail beyond line = potential noise issues' ...
    }, ...
    'FitBoxToText','on', ...
    'BackgroundColor','black');
