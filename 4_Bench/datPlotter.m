function out = datPlotter(argv)

filesTab = readtable(argv, 'Delimiter', '\n');

disp(filesTab)

slash = strfind(argv, '/');
folder = extractBefore(argv, slash(end)); %% use last '/' to get path

fid = fopen(folder + "/faultyTests.txt", "at");


for i = 1:height(filesTab)
    file = char(filesTab.(1)(i));
    disp("Working on .dat file: " + file);
    if (analizeDatFile(file) == -1)
        disp("Error in " + file + " ; -1 found.")
        % we save the file on a list of failed tests
        fprintf(fid, file + '\n');
    end
end

fclose(fid);

fprintf('\n');
disp("######################")
disp("# Elaboration ended. #")
disp("######################")

out = 0;
end

function sfile = analizeDatFile(file)

datValue = retrieveData(file);

if (any(datValue.(1) == -1))
    % we have at least an error in a sequence of tests
    sfile = -1;
    return;
end

for i = 2+1: 5 % the first part of graphs
    disp("Start printing Graph n° " + i)
    fig = printGraph(datValue(:, [1 2 i]), i);
    disp("Printing Graph " + i + " done")
    
    dot = strfind(file, '.');
    folder = extractBefore(file, dot(end)) + "/"; %% use last '.' to get path
    
    w = warning('off','all');
    mkdir(folder);
    warning(w);
    
    filename = datValue(:,i).Properties.VariableNames(1);
    
    path = folder + filename;
    disp("Saving in: " + path + ".png")
    saveas(fig, path, 'png');
end
fprintf('\n')

disp("Now saving the time response graph...")

nParCon = getParConn(datValue);

for con = 1:length(nParCon)
    rows = datValue.(2) == nParCon(con);
    resp_fig = printResponseTime(datValue(rows, :));
    
    path = folder + "TimeResp_" + nParCon(con);
    disp("Saving Response graph for " + nParCon(con) + ...
        " parallel connections in: ")
    fprintf('\t');
    disp(path + ".png")
    saveas(resp_fig, path, 'png');
end

disp("Analysis of .dat file done.")

sfile = 0;
end

function tab = retrieveData(path)
% to retrieve numbers data
tab = readtable(path, 'VariableNamingRule','preserve');

end

function figObj = printGraph(subDat, index)

figObj = figure(index);
% set(figObj, 'Visible', 'off'); % uncomment to avoid figure popup

PlotName = subDat(:,1).Properties.VariableNames(1);
XName = subDat(:,2).Properties.VariableNames(1);
YName = subDat(:,3).Properties.VariableNames(1);

nCore = subDat.(1)(end); % max number of core
while(nCore > 0)
    
    c = subDat.(1);
    
    [indexes] = find(c == nCore);
    
    if (isempty(indexes)) % if we don't have considered this amount of core
        nCore = nCore - 1;
        continue;
    end
    
    valueTab = table2array( subDat(indexes(1):indexes(end),2:end) );
    
    subDat = subDat(1:indexes(1)-1, :); % we reduce size of subDat for the next iteration of find
    
    plot(valueTab(:,1), valueTab(:,2), '-*', ...
        'DisplayName', PlotName + ": " + nCore);
    grid on
    hold on
    
    if(indexes(1) == 1) % we reached the minimum amount of core
        break;
    end
    
    nCore = nCore - 1;
end
% analyzed all data, adding some info and save on file

hold off

xlabel(XName)
ylabel(YName)

% if(valueTab(1,2) < valueTab(end,2)) %% the graph is increasing
%     position = 'Northwest';
% else
%     position = 'Northeast';
% end

legend('Interpreter','none', 'Location','best');

end

function parConn = getParConn(datValue)

indexConn = find(datValue.(1) == 1);

tmp = indexConn; % same array size

for ind = 1:length(indexConn)
    tmp(ind) = datValue.(2)(ind);
end

parConn = tmp;

end

function resp_fig = printResponseTime(reducedDat)
%reducedDat is only shorter, not slimmer

resp_fig = figure(22);
% set(resp_fig, 'Visible', 'off'); % uncomment to avoid figure popup

numConn = reducedDat.(2)(1);

% we have to discard some columns for plotting
valueTab = table2array(reducedDat(:, [1 6:end]));

percentage = [50 66 75 80 90 95 98 99 100]; % WARNING, w/o min in this case

for row = 1: height(valueTab)
    % using 2 below, we include minimum response time! [default = 3]
    plot(percentage, valueTab(row,3:end), '-*', ...
        'DisplayName', "N°Worker: " + valueTab(row,1));
    grid on
    hold on 
end

hold off

title("Time Response with " + numConn + " parallel connections")
ylabel("Time [ms]")
xlabel("Percentage [%]")

legend('Interpreter','none', 'Location','best');

end