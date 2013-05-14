% Author: Yang Zhao
% Stanford University
% Reference: Snoopy 2011

%Input: img is a binarized image, with black background and white words
function bbArray = word_segment(img)
[imL numL] = bwlabel(img);
regions = regionprops(imL, 'BoundingBox');
bb = zeros(numL, 4);
for i = 1 : numL
    bb(i,:) = regions(i).BoundingBox;
%     rectangle('Position',bb(i,:),'Linewidth',2,'EdgeColor','red');
end

%----------------------------------
% Fix i's and j's dot
%----------------------------------
avg_x = mean(bb(:,3));
avg_y = mean(bb(:,4));
avg_A = avg_x * avg_y;  
Area = bb(:,3) .* bb(:,4);                  % Find area of all bounding boxes
idx = Area < 0.2 * avg_A; 

ibb = bb(idx,:);                %small areas
sNum = size(ibb, 1);
for i = 1 : sNum                             % For each of the small areas
    df = bb - repmat(ibb(i,:),numL,1);
    ydf = df(:,2) < avg_y * 0.9 & df(:,2) > 0;  
    xdf = abs(df(:,1)) < avg_y * 0.4 & df(:,1) ~=0;
    mrg = xdf & ydf;    %means they could be merged
    if(sum(mrg) == 1)
        bb(mrg,:) = [min(bb(mrg,1),ibb(i,1)),min(bb(mrg,2),ibb(i,2)),bb(mrg,3), bb(mrg,4)+(bb(mrg,2)-ibb(i,2))];
    end
end


bb(idx,:) = [];   %logical index causes bb automatically shrink
figure(3); imshow(img);
% for i = 1 : numL - sNum
%     rectangle('Position',bb(i,:),'Linewidth',2,'EdgeColor','red');
% end

%----------------------------------------------------
% Recalculate parameters of bounding box of letters
%----------------------------------------------------
avg_x = sum(bb(:,3))/length(bb);            % Recalculating the avg width separation
avg_y = sum(bb(:,4))/length(bb);            % Recalculating the avg height separation
LetSep = 0.31;                               % 30% avg letter separation   
BB = [];                                    % Bounding box for final set of words
unbb = bb;                                  % Unsorted bounding box

%--------------------------------------------------------------------------
%   sort coordinates to have words in the same line
%--------------------------------------------------------------------------
while(~isempty(unbb))                           % There are lines to be extracted
    min_y = min(unbb(:,2));                     % Get the minimum line position
    idx = abs(unbb(:,2) - min_y ) < 1.8 *avg_y; % Get co-ordinates of y in same line
    bbLine = unbb(idx,:);                           % Get words in a line
    unbb(idx,:) = [];                           % Remove the line and prep for next line
    while(~isempty(bbLine))                         % There are words to be extracted
        [rw ~] = size(bbLine);
        DF = bbLine(:,1:2) - (repmat(bbLine(1,1:2),rw,1) + bbLine(1,3)*[ones(rw,1) zeros(rw,1)]);
        xDF = DF(:,1) < avg_y * LetSep & DF(:,1) >= -round(0.022*avg_y);
        yDF = DF(:,2) < avg_y*(1+0.3);
        nxtLt = xDF & yDF;
        nxtLt(1) = 0;
        if (sum(nxtLt) == 0)                                                                    % Word or funky stuff has been isolated
            BB = [BB;bbLine(1,:)];
            bbLine(1,:) = [];
        elseif(sum(nxtLt) == 1)                                                                  % Next letter found
            ux = bbLine(1,1);                                                                        % Update x
            uy = min(bbLine(1,2),bbLine(nxtLt,2));                                                       % Update y
            uw = bbLine(1,3) + bbLine(nxtLt,3) + abs(bbLine(1,1)+bbLine(1,3) - bbLine(nxtLt,1));                     % Update width
            if(uy+bbLine(1,4) < bbLine(nxtLt,2) + bbLine(nxtLt,4) )
                uh = bbLine(1,4) -( uy+bbLine(1,4))+  bbLine(nxtLt,2) + bbLine(nxtLt,4);
            else
                uh = bbLine(1,4) ; % Update height
            end
            uc = [ux uy uw uh];                                                                  % Updated co-ordinates
            % Remove bounding boxes that were merged
            bbLine(nxtLt,:) = [];
            bbLine(1,:) = [];
            bbLine = [uc;bbLine];
        else
            idx = find(nxtLt == 1,1);
            nxtLt = zeros(rw,1); nxtLt(idx) = 1; nxtLt = logical(nxtLt);
            ux = bbLine(1,1);                                                                        % Update x
            uy = min(bbLine(1,2),bbLine(nxtLt,2));                                                       % Update y
            uw = bbLine(1,3) + bbLine(nxtLt,3) + abs(bbLine(1,1)+bbLine(1,3) - bbLine(nxtLt,1));                     % Update width
            if(uy+bbLine(1,4) < bbLine(nxtLt,2) + bbLine(nxtLt,4) )
                uh = bbLine(1,4) -( uy+bbLine(1,4))+  bbLine(nxtLt,2) + bbLine(nxtLt,4);
            else
                uh = bbLine(1,4) ; % Update height
            end
            uc = [ux uy uw uh];                                                                  % Updated co-ordinates
            % Remove bounding boxes that were merged
            bbLine(nxtLt,:) = [];
            bbLine(1,:) = [];
            bbLine = [uc;bbLine]; 
        end
        clear DF;clear xDF;clear yDF;clear nxtLt; 
    end
    clear bbLine;
end

%------------------------------------------------------
% Output the coordinate of bounding boxes
%------------------------------------------------------
bbArray = uint16(BB);
end
