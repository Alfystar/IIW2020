% manual function to create merged plot with the two subsystems.
% to obtain the #response and kbps, it's necessary to uncomment
% the printGraph part, otherwise only timeResponses will be
% exported to a pdf file
function out = plotter(path)

if (nargin ~= 1)
    disp("Error in plotter, naragin ~= 1")
    return;
end

analizeDatFile(path);


out=0;
end

function sfile = analizeDatFile(file)

datValue = retrieveData(file);

for i = 2+1: 5 % the first part of graphs
    %     disp("Start printing Graph n° " + i)
    %     fig = printGraph(datValue(:, [1 2 i]), i, file);
    %     disp("Printing Graph " + i + " done")
    
    dot = strfind(file, '.');
    folder = extractBefore(file, dot(end)) + "/"; %% use last '.' to get path
    
    w = warning('off','all');
    mkdir(folder);
    mkdir(folder + 'TimeResponse/');
    
    warning(w);
    
    %     filename = datValue(:,i).Properties.VariableNames(1);
    %
    %     path = folder + filename + ".pdf";
    %     disp("Saving in: " + path + ".pdf")
    %     exportgraphics(fig, path,'ContentType','vector')
end
fprintf('\n')

disp("Now saving the time response graph...")

nParCon = getParConn(datValue);

for con = 1:length(nParCon)
    rows = datValue.(2) == nParCon(con);
    resp_fig = printResponseTime(datValue(rows, :), file);
    
    path = folder + 'TimeResponse/' + "TimeResp_" + nParCon(con);
    disp("Saving Response graph for " + nParCon(con) + ...
        " parallel connections in: ")
    fprintf('\t');
    disp(path + ".png")
    exportgraphics(resp_fig, path + '.pdf','ContentType','vector')
end

disp("Analysis of .dat file done.")

sfile = 0;
end

function tab = retrieveData(path)
% to retrieve numbers data

tab = benchMarkTable(path);
end

function figObj = printGraph(subDat, index, fileName)

figObj = figure(index);

PlotName = subDat(:,1).Properties.VariableNames(1);
XName = subDat(:,2).Properties.VariableNames(1);
YName = subDat(:,3).Properties.VariableNames(1);
nMaxCore = subDat.(1)(end); % max number of core


while(nMaxCore > 0)
    
    c = subDat.(1);
    [indexes] = find(c == nMaxCore);
    
    if (isempty(indexes)) % if we don't have considered this amount of core
        nMaxCore = nMaxCore - 1;
        continue;
    end
    
    if (nMaxCore == 20)
        displayName = "Apache";
    else
        displayName = "N° Worker: " + nMaxCore;
    end
    
    valueTab = table2array( subDat(indexes(1):indexes(end),2:end) );
    
    subDat = subDat(1:indexes(1)-1, :); % we reduce size of subDat for the next iteration of find
    
    plot(valueTab(:,1), valueTab(:,2), '-*', 'DisplayName', displayName);
    grid on
    hold on
    
    if(indexes(1) == 1) % we reached the minimum amount of core
        break;
    end
    
    nMaxCore = nMaxCore - 1;
end

hold off

% add some labels
xlabel(XName)
ylabel(YName)

slash = strfind(fileName, '/');
title_file = extractAfter(fileName, slash(end));

dot = strfind(title_file, '.');
title_file = extractBefore(title_file, dot(end));
title(title_file, "Interpreter", "none")

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

function resp_fig = printResponseTime(reducedDat, fileName)
%reducedDat is only shorter, not slimmer

resp_fig = figure(22);

numConn = reducedDat.(2)(1);

% we have to discard some columns for plotting
valueTab = table2array(reducedDat(:, [1 7:end]));

percentage = [50 66 75 80 90 95 98 99 100]; % WARNING, w/o min in this case

for row = 1: size(valueTab,1)
    
    for i = 1:length(valueTab(row,2:end))
        if(valueTab(row,i) == 0)
            valueTab(row,i) = 1; % Il diagramma semilogaritmico non vuole 0
        end
    end
    
    if (valueTab(row,1)==20)
        displayName= "Apache";
    else
        displayName = "N°Worker: " + valueTab(row,1);
    end
    plot(valueTab(row,2:end),percentage, '-*', 'DisplayName', displayName);
    hold on
    grid on
end

hold off

slash = strfind(fileName, '/');
file = extractAfter(fileName, slash(end));
dot = strfind(file, '.');
title_file = extractBefore(file, dot(end));

title("'" + title_file + "' : " + "Time Response, " + numConn + " Conns", "Interpreter", "none")

xlabel("Time [ms]")
ylabel("Percentage [%]")

legend('Interpreter','none', 'Location','best');

end